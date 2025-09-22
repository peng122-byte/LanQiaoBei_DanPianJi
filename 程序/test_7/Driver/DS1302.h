#ifndef __DS1302_H
#define __DS1302_H
#include <intrins.H>
#include <STC15F2K60S2.H>

sbit SCK = P1^7;
sbit SDA = P2^3;
sbit RST = P1^3;

void Write_DS1302(unsigned char temp);
void Write_DS1302_Byte(unsigned char address, unsigned char dat);
unsigned char Read_DS1302_Byte(unsigned char address);
void Set_Rtc(unsigned char* ucRtc);
void Read_Rtc(unsigned char* ucRtc);
#endif
