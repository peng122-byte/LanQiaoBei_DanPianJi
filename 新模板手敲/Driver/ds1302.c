/**
 * @file    ds1302.c
 * @brief   DS1302实时时钟驱动模块
 * @details DS1302是低功耗实时时钟芯片，通过三线SPI接口通信(SCK/SDA/RST)
 *          数据以BCD码格式存储，读写时需要进行BCD与十进制的转换
 *          寄存器地址：时=0x84/0x85, 分=0x82/0x83, 秒=0x80/0x81 (写/读)
 *
 *          本驱动代码基于蓝桥杯官方提供的代码片段修改
 */
#include "ds1302.h"

/* DS1302三线接口引脚定义 */
sbit SCK = P1^7;    // 时钟线
sbit SDA = P2^3;    // 数据线(双向)
sbit RST = P1^3;    // 复位/片选线(高电平有效)

/**
 * @brief   向DS1302写入一个字节(底层时序)
 * @param   temp  要写入的字节数据
 * @note    数据在SCK上升沿被DS1302锁存，LSB先发送
 */
void Write_Ds1302(unsigned char temp)
{
	unsigned char i;
	for (i=0; i<8; i++)
	{
		SCK = 0;
		SDA = temp & 0x01;   // 取最低位发送(LSB优先)
		temp >>= 1;          // 右移准备下一位
		SCK = 1;             // 上升沿锁存数据
	}
}

/**
 * @brief   向DS1302指定地址写入一个字节
 * @param   address  寄存器地址(写地址，最低位为0)
 * @param   dat      要写入的数据
 * @note    操作时序：RST拉高 -> 写地址 -> 写数据 -> RST拉低
 */
void Write_Ds1302_Byte(unsigned char address, unsigned char dat)
{
	RST = 0; _nop_();
	SCK = 0; _nop_();
	RST = 1; _nop_();        // 片选有效
	Write_Ds1302(address);   // 发送寄存器地址
	Write_Ds1302(dat);       // 发送数据
	RST = 0;                 // 结束通信
}

/**
 * @brief   从DS1302指定地址读取一个字节
 * @param   address  寄存器地址(读地址，最低位为1)
 * @return  读取到的字节数据
 * @note    写地址后，在SCK下降沿读取SDA数据，LSB优先
 */
unsigned char Read_Ds1302_Byte(unsigned char address)
{
	unsigned char i, temp = 0x00;
	RST = 0; _nop_();
	SCK = 0; _nop_();
	RST = 1; _nop_();         // 片选有效
	Write_Ds1302(address);    // 发送读地址

	for (i=0; i<8; i++)       // 逐位读取数据
	{
		SCK = 0;
		temp >>= 1;           // 右移腾出最高位
		if(SDA)
			temp |= 0x80;     // SDA为高则最高位置1
		SCK = 1;
	}

	/* 通信结束，释放总线 */
	RST = 0; _nop_();
	SCK = 0; _nop_();
	SCK = 1; _nop_();
	SDA = 0; _nop_();
	SDA = 1; _nop_();

	return temp;
}

/**
 * @brief   设置DS1302时间(十进制 -> BCD码写入)
 * @param   ucRtc  3字节数组 [时, 分, 秒]，十进制格式(如 12,23,45)
 * @note    写入流程：关写保护 -> 停止计时 -> 写入时分秒 -> 开写保护
 *          十进制转BCD：例如 23 -> 0x23，即 23/10*16 + 23%10 = 35 = 0x23
 */
void Set_Rtc(unsigned char *ucRtc)
{
	unsigned char i;
	Write_Ds1302_Byte(0x8e, 0x00);  // 关闭写保护
	Write_Ds1302_Byte(0x80, 0x80);  // 暂停振荡器(CH位=1)，停止计时

	for(i=0; i<3; i++)
	{
		/* 地址：时=0x84, 分=0x82, 秒=0x80 */
		Write_Ds1302_Byte(0x84 - 2*i, ucRtc[i]/10*16 + ucRtc[i]%10);
	}

	Write_Ds1302_Byte(0x8e, 0x80);  // 开启写保护(恢复计时由秒寄存器CH位控制)
}

/**
 * @brief   读取DS1302时间(BCD码 -> 十进制)
 * @param   ucRtc  3字节数组 [时, 分, 秒]，读取后为十进制格式
 * @note    读取时关闭中断EA，防止读取过程被打断导致数据不一致
 *          BCD转十进制：例如 0x23 -> 23，即 0x23/16*10 + 0x23%16 = 23
 */
void Read_Rtc(unsigned char *ucRtc)
{
	unsigned char i;
	unsigned char temp;
	EA = 0;                         // 关中断，保证读取原子性

	for(i=0; i<3; i++)
	{
		/* 读地址：时=0x85, 分=0x83, 秒=0x81 */
		temp = Read_Ds1302_Byte(0x85 - 2*i);
		ucRtc[i] = temp/16*10 + temp%16;  // BCD码转十进制
	}

	EA = 1;                         // 恢复中断
}
