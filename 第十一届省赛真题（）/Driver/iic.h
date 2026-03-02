/**
 * @file    iic.h
 * @brief   I2C总线驱动头文件(PCF8591 ADC/DAC + AT24C02 EEPROM)
 */
#ifndef _IIC_H_
#define _IIC_H_
#include <STC15F2K60S2.H>

/* PCF8591 AD/DA */
unsigned char Ad_Read(unsigned char addr);     /* AD转换，addr=通道地址，返回0~255 */
void Da_Write(unsigned char dat);              /* DA输出，dat=0~255对应0~5V */

/* AT24C02 EEPROM */
void EEPROM_Write(unsigned char *str, unsigned char addr, unsigned num);  /* 连续写入 */
void EEPROM_Read(unsigned char *str, unsigned char addr, unsigned num);   /* 连续读取 */

#endif
