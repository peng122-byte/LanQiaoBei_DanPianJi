/**
 * @file    iic.c
 * @brief   I2C总线驱动模块(PCF8591 ADC/DAC + AT24C02 EEPROM)
 * @details 本模块包含：
 *          1. I2C底层时序(Start/Stop/SendByte/ReceiveByte/Ack)
 *          2. PCF8591 AD转换(4通道模拟输入，8位精度，0~5V -> 0~255)
 *          3. PCF8591 DA转换(1通道模拟输出，8位精度，0~255 -> 0~5V)
 *          4. AT24C02 EEPROM读写(256字节容量，I2C地址0xA0)
 *
 *          PCF8591通道说明(CT107D开发板)：
 *            0x40/0x00: 外部输入
 *            0x41/0x01: 光敏电阻
 *            0x42/0x02: 差分输入
 *            0x43/0x03: 滑动变阻器
 *
 *          本驱动代码基于蓝桥杯官方提供的代码片段修改
 */
#include "iic.h"
#include "intrins.h"

#define DELAY_TIME  5          // I2C时序延时参数

/* I2C总线引脚定义 */
sbit scl = P2^0;              // 时钟线
sbit sda = P2^1;              // 数据线

/* ==================== I2C底层时序 ==================== */

/**
 * @brief   I2C延时函数
 * @param   n  延时循环次数
 */
static void I2C_Delay(unsigned char n)
{
	do
	{
		_nop_();_nop_();_nop_();_nop_();_nop_();
		_nop_();_nop_();_nop_();_nop_();_nop_();
		_nop_();_nop_();_nop_();_nop_();_nop_();
	}
	while(n--);
}

/**
 * @brief   I2C起始信号
 * @note    时序：SCL高电平期间，SDA由高变低
 */
void I2CStart(void)
{
	sda = 1;
	scl = 1;
	I2C_Delay(DELAY_TIME);
	sda = 0;                   // SDA下降沿 = 起始信号
	I2C_Delay(DELAY_TIME);
	scl = 0;                   // 钳住总线，准备发送数据
}

/**
 * @brief   I2C停止信号
 * @note    时序：SCL高电平期间，SDA由低变高
 */
void I2CStop(void)
{
	sda = 0;
	scl = 1;
	I2C_Delay(DELAY_TIME);
	sda = 1;                   // SDA上升沿 = 停止信号
	I2C_Delay(DELAY_TIME);
}

/**
 * @brief   I2C发送一个字节
 * @param   byt  要发送的字节(MSB优先)
 * @note    数据在SCL低电平时准备，SCL高电平时被从机采样
 */
void I2CSendByte(unsigned char byt)
{
	unsigned char i;

	for(i=0; i<8; i++){
		scl = 0;
		I2C_Delay(DELAY_TIME);
		if(byt & 0x80){        // 取最高位
			sda = 1;
		}
		else{
			sda = 0;
		}
		I2C_Delay(DELAY_TIME);
		scl = 1;               // SCL上升沿，从机采样数据
		byt <<= 1;             // 左移准备下一位
		I2C_Delay(DELAY_TIME);
	}

	scl = 0;                   // 释放时钟线
}

/**
 * @brief   I2C接收一个字节
 * @return  接收到的字节数据
 * @note    主机释放SDA，在SCL高电平期间读取从机数据(MSB优先)
 */
unsigned char I2CReceiveByte(void)
{
	unsigned char da;
	unsigned char i;
	for(i=0; i<8; i++){
		scl = 1;
		I2C_Delay(DELAY_TIME);
		da <<= 1;              // 左移腾出最低位
		if(sda)
			da |= 0x01;       // SDA为高则最低位置1
		scl = 0;
		I2C_Delay(DELAY_TIME);
	}
	return da;
}

/**
 * @brief   I2C等待应答
 * @return  0=收到ACK(应答), 1=收到NACK(非应答)
 */
unsigned char I2CWaitAck(void)
{
	unsigned char ackbit;

	scl = 1;
	I2C_Delay(DELAY_TIME);
	ackbit = sda;              // 读取从机应答位(低电平=ACK)
	scl = 0;
	I2C_Delay(DELAY_TIME);

	return ackbit;
}

/**
 * @brief   I2C发送应答/非应答
 * @param   ackbit  0=发送ACK(继续接收), 1=发送NACK(停止接收)
 */
void I2CSendAck(unsigned char ackbit)
{
	scl = 0;
	sda = ackbit;              // 设置应答位
	I2C_Delay(DELAY_TIME);
	scl = 1;                   // SCL高电平，从机读取应答
	I2C_Delay(DELAY_TIME);
	scl = 0;
	sda = 1;                   // 释放数据线
	I2C_Delay(DELAY_TIME);
}

/* ==================== PCF8591 AD/DA转换 ==================== */

/**
 * @brief   PCF8591 DA转换(数字量 -> 模拟量)
 * @param   dat  8位DA值(0~255，对应0~5V模拟输出)
 * @note    控制字0x41开启DA输出功能
 *          电压计算：V_out = dat / 255 * 5V
 *          例如：Da_Write(3*51) 输出约3V
 */
void Da_Write(unsigned char dat)
{
	I2CStart();
	I2CSendByte(0x90);         // 选中PCF8591，写操作
	I2CWaitAck();
	I2CSendByte(0x41);         // 控制字：开启DA输出
	I2CWaitAck();
	I2CSendByte(dat);          // 写入DA数值
	I2CWaitAck();
	I2CStop();
}
