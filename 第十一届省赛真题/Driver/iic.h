/**
 * @file    iic.h
 * @brief   I2C总线驱动头文件(PCF8591 ADC/DAC + AT24C02 EEPROM)
 */
#ifndef _IIC_H_
#define _IIC_H_
#include <STC15F2K60S2.H>

/* PCF8591 AD/DA */

void Da_Write(unsigned char dat);              /* DA输出，dat=0~255对应0~5V */

\

#endif
