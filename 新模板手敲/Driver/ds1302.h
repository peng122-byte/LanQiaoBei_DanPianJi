#ifndef _DS1302_H_
#define _DS1302_H_
#include <STC15F2K60S2.H>
#include <intrins.h>
void Set_Rtc(unsigned char *ucRtc);
void Read_Rtc(unsigned char *ucRtc);
#endif