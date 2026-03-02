/**
 * @file    onewire.h
 * @brief   单总线(DS18B20温度传感器)驱动头文件
 */
#ifndef _ONEWIRE_H_
#define _ONEWIRE_H_
#include <STC15F2K60S2.H>

float rd_temperature(void);   /* 读取温度值，返回float类型(单位°C) */

#endif
