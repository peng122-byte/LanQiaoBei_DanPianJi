/**
 * @file    ds1302.h
 * @brief   DS1302实时时钟驱动头文件
 */
#ifndef _DS1302_H_
#define _DS1302_H_
#include <STC15F2K60S2.H>
#include <intrins.h>

void Set_Rtc(unsigned char *ucRtc);    /* 设置时间，传入[时,分,秒]十进制数组 */
void Read_Rtc(unsigned char *ucRtc);   /* 读取时间，返回[时,分,秒]十进制数组 */

#endif
