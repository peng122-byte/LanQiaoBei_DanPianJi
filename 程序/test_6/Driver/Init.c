#include <Init.H>
void System_Init()
{
	P0 = 0xff;//Led
	P2 = P2 & 0x1f | 0x80;//锁存器开门
	P2 &= 0x1f;//锁存器关门
	
	P0 = 0x00;//继电器、蜂鸣器
	P2 = P2 & 0x1f | 0xa0;//锁存器开门
	P2 &= 0x1f;//锁存器关门
}
