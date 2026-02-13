/*	#   I2C代码片段说明
	1. 	本文件夹中提供的驱动代码供参赛选手完成程序设计参考。
	2. 	参赛选手可以自行编写相关代码或以该代码为基础，根据所选单片机类型、运行速度和试题
		中对单片机时钟频率的要求，进行代码调试和修改。
*/
#include "iic.h"
#include "intrins.h"
#define DELAY_TIME	5 //10

sbit scl = P2^0;
sbit sda = P2^1;
//
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

//
void I2CStart(void)
{
    sda = 1;
    scl = 1;
	I2C_Delay(DELAY_TIME);
    sda = 0;
	I2C_Delay(DELAY_TIME);
    scl = 0;    
}

//
void I2CStop(void)
{
    sda = 0;
    scl = 1;
	I2C_Delay(DELAY_TIME);
    sda = 1;
	I2C_Delay(DELAY_TIME);
}

//
void I2CSendByte(unsigned char byt)
{
    unsigned char i;
	
    for(i=0; i<8; i++){
        scl = 0;
		I2C_Delay(DELAY_TIME);
        if(byt & 0x80){
            sda = 1;
        }
        else{
            sda = 0;
        }
		I2C_Delay(DELAY_TIME);
        scl = 1;
        byt <<= 1;
		I2C_Delay(DELAY_TIME);
    }
	
    scl = 0;  
}

//
unsigned char I2CReceiveByte(void)
{
	unsigned char da;
	unsigned char i;
	for(i=0;i<8;i++){   
		scl = 1;
		I2C_Delay(DELAY_TIME);
		da <<= 1;
		if(sda) 
			da |= 0x01;
		scl = 0;
		I2C_Delay(DELAY_TIME);
	}
	return da;    
}

//
unsigned char I2CWaitAck(void)
{
	unsigned char ackbit;
	
    scl = 1;
	I2C_Delay(DELAY_TIME);
    ackbit = sda; 
    scl = 0;
	I2C_Delay(DELAY_TIME);
	
	return ackbit;
}

//
void I2CSendAck(unsigned char ackbit)
{
    scl = 0;
    sda = ackbit; 
	I2C_Delay(DELAY_TIME);
    scl = 1;
	I2C_Delay(DELAY_TIME);
    scl = 0; 
	sda = 1;
	I2C_Delay(DELAY_TIME);
}

/*
开启4就代表要开启DA
	0x40/0x00外部输入
	0x41/0x01光敏电阻
	0x42/0x02差分输入
	0x43/0x03滑动变阻器输入
*/
//0-5的模拟量转换为0-255
unsigned char Ad_Read(unsigned char addr)
{
	unsigned char temp;
	
	I2CStart();
	I2CSendByte(0x90);//选中PCF,并且写操作
	I2CWaitAck();
	I2CSendByte(addr);//写入想要转换的地址
	I2CWaitAck();
	
	I2CStart();
	I2CSendByte(0x91);//选中PCF，并且读操作
	I2CWaitAck();
	temp = I2CReceiveByte();
	
	I2CSendAck(1);
	I2CStop();
	
	return temp;
}

//0-255的数字量转换为0-5
void Da_Write(unsigned char dat)
{
	I2CStart();
	I2CSendByte(0x90);//选中PCF,并且写操作
	I2CWaitAck();
	I2CSendByte(0x41);//写入DA使能
	I2CWaitAck();
	I2CSendByte(dat);//写入数字电压，DA可以转换为模拟电压
	I2CWaitAck();
	I2CStop();
}

//写入的数组，写入的起始位置，写入数据的个数（字节数）
void EEPROM_Write(unsigned char *str,unsigned char addr,unsigned num)
{
	I2CStart();
	I2CSendByte(0xa0);//选中AT24C02,并且写操作
	I2CWaitAck();
	I2CSendByte(addr);//写入起始地址
	I2CWaitAck();
	
	
	while(num--)
	{
		I2CSendByte(*str++);//写入我们的每一位数据后指针递增,然后为下一次写数据准备
		I2CWaitAck();
		I2C_Delay(200);//保证完全写入
	}
	I2CStop();
	I2C_Delay(255);
	I2C_Delay(255);
	I2C_Delay(255);
	I2C_Delay(255);
	I2C_Delay(255);
	I2C_Delay(255);
	I2C_Delay(255);
	I2C_Delay(255);
	I2C_Delay(255);
	I2C_Delay(255);
}

//读取的数组，读取的起始位置，读取数据的个数（字节数）
void EEPROM_Read(unsigned char *str,unsigned char addr,unsigned num)
{
	I2CStart();
	I2CSendByte(0xa0);//选中AT24C02,并且写操作
	I2CWaitAck();
	I2CSendByte(addr);//写入起始地址
	I2CWaitAck();
	
	I2CStart();
	I2CSendByte(0xa1);//选中AT24C02,并且读取操作
	I2CWaitAck();
	EA=0;
	while(num--)
	{
		*str ++ = I2CReceiveByte();
		if(num)
			I2CSendAck(0);
		else
			I2CSendAck(1);
	}
	I2CStop();
	EA=1;
}