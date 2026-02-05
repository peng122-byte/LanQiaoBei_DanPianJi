#include "init.h"
void System_Init()
{
	unsigned char temp;
	//关闭LED
	P0 = 0xff;
	
	temp = P2 & 0x1f;
	temp = temp|0x80;
	P2 = temp;
	temp = P2 & 0x1f;
	P2 = temp;
	
	//关闭继电器，蜂鸣器，电机
	P0 = 0x00;
	temp = P2 & 0x1f;
	temp = temp|0xa0;
	P2 = temp;
	temp = P2 & 0x1f;
	P2 = temp;
	
	
}