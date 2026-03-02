/**
 * @file    onewire.c
 * @brief   单总线(1-Wire)驱动模块 - DS18B20温度传感器
 * @details DS18B20是数字温度传感器，通过单总线协议通信
 *          测量范围：-55~+125°C，精度：0.0625°C(12位分辨率)
 *          通信流程：复位 -> ROM命令 -> 功能命令
 *
 *          本驱动代码基于蓝桥杯官方提供的代码片段修改
 */
#include "onewire.h"

sbit DQ = P1^4;    // 单总线数据引脚

/**
 * @brief   单总线延时函数
 * @param   t  延时参数，每个单位约1us(12MHz晶振下)
 * @note    延时精度直接影响通信时序，需根据晶振频率调整内部循环
 */
void Delay_OneWire(unsigned int t)
{
	unsigned char i;
	while(t--){
		for(i=0; i<12; i++);  // 12MHz下约1us
	}
}

/**
 * @brief   向DS18B20写入一个字节
 * @param   dat  要写入的字节数据
 * @note    写时序(LSB优先)：
 *          写0：拉低DQ >60us
 *          写1：拉低DQ <15us，然后释放
 */
void Write_DS18B20(unsigned char dat)
{
	unsigned char i;
	for(i=0; i<8; i++)
	{
		DQ = 0;              // 拉低总线，启动写时隙
		DQ = dat & 0x01;     // 发送最低位
		Delay_OneWire(5);    // 保持足够时间让DS18B20采样
		DQ = 1;              // 释放总线
		dat >>= 1;           // 右移准备下一位
	}
	Delay_OneWire(5);
}

/**
 * @brief   从DS18B20读取一个字节
 * @return  读取到的字节数据
 * @note    读时序(LSB优先)：主机拉低DQ启动读时隙，然后释放，
 *          在15us内采样DQ电平
 */
unsigned char Read_DS18B20(void)
{
	unsigned char i;
	unsigned char dat;

	for(i=0; i<8; i++)
	{
		DQ = 0;              // 拉低总线，启动读时隙
		dat >>= 1;           // 右移腾出最高位
		DQ = 1;              // 释放总线，等待DS18B20输出数据
		if(DQ)
		{
			dat |= 0x80;     // DQ为高则最高位置1
		}
		Delay_OneWire(5);    // 等待时隙结束
	}
	return dat;
}

/**
 * @brief   DS18B20复位/存在检测
 * @return  0=存在(正常), 1=不存在(异常)
 * @note    复位时序：主机拉低DQ 480us以上 -> 释放 -> 等待60~240us ->
 *          检测DS18B20拉低DQ的应答脉冲
 */
bit init_ds18b20(void)
{
	bit initflag = 0;

	DQ = 1;
	Delay_OneWire(12);
	DQ = 0;                  // 主机拉低总线 >480us
	Delay_OneWire(80);
	DQ = 1;                  // 释放总线
	Delay_OneWire(10);       // 等待DS18B20应答
	initflag = DQ;           // 0=DS18B20存在(它会拉低总线)
	Delay_OneWire(5);

	return initflag;
}

/**
 * @brief   读取DS18B20温度值
 * @return  温度值(float类型，单位°C，精度0.0625°C)
 * @note    完整流程：
 *          1. 复位 -> 跳过ROM(0xCC) -> 启动转换(0x44)
 *          2. 等待转换完成(约200个延时单位)
 *          3. 复位 -> 跳过ROM(0xCC) -> 读取暂存器(0xBE)
 *          4. 先读低字节，再读高字节
 *          5. 合并后乘以0.0625得到实际温度
 */
float rd_temperature()
{
	unsigned char high, low;

	/* 第一步：启动温度转换 */
	init_ds18b20();
	Write_DS18B20(0xcc);     // 跳过ROM检查(单设备总线)
	Write_DS18B20(0x44);     // 启动温度转换

	Delay_OneWire(200);      // 等待转换完成

	/* 第二步：读取温度数据 */
	init_ds18b20();
	Write_DS18B20(0xcc);     // 跳过ROM检查
	Write_DS18B20(0xbe);     // 读取暂存器命令

	low = Read_DS18B20();    // 读温度低字节
	high = Read_DS18B20();   // 读温度高字节

	/* 合并高低字节，乘以分辨率0.0625°C */
	return (float)(high << 8 | low) * 0.0625;
}
