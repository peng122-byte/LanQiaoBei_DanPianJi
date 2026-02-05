#include <STC15F2K60S2.H>
#include "LED.h"
#include "init.h"
#include "Seg.h"
#include "Key.h"
pdata unsigned char ucLed[8] = {1,0,1,0,1,0,1,0};
pdata unsigned char Seg_Buf[8] = {10,10,10,10,10,10,10,10};
idata unsigned char Seg_Pos = 0;
idata unsigned char Seg_Slow_Down;

idata unsigned char Key_Val,Key_Old,Key_Up,Key_Down;
idata unsigned char Key_Slow_Down;
void Led_Proc()
{
	ucLed[0] = 1;
	ucLed[1] = 0;
	ucLed[2] = 1;
	ucLed[3] = 0;
	ucLed[4] = 1;
	ucLed[5] = 0;
	ucLed[6] = 1;
	ucLed[7] = 0;
	Led_Disp(ucLed);
}
void Key_Proc()
{
	Seg_Buf[0] = 1;
	Seg_Buf[1] = 2;
	Seg_Buf[2] = 3+',';
	Seg_Buf[3] = 4;
	Seg_Buf[4] = 5;
	Seg_Buf[5] = 6;
	Seg_Buf[6] = 7;
	Seg_Buf[7] = 8;
}
void Seg_Proc()
{
	if(Seg_Slow_Down < 20) return;
	
}

void Timer1_Isr(void) interrupt 3
{
	Seg_Slow_Down ++;
	Seg_Pos = (++Seg_Pos) % 8;
	if(Seg_Buf[Seg_Pos] > 20) 
	{
		Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos]- ',',1);
	}else
	{
		Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos],0);
	}
}

void Timer1_Init(void)		//1毫秒@12.000MHz
{
	AUXR &= 0xBF;			//定时器时钟12T模式
	TMOD &= 0x0F;			//设置定时器模式
	TL1 = 0x18;				//设置定时初始值
	TH1 = 0xFC;				//设置定时初始值
	TF1 = 0;				//清除TF1标志
	TR1 = 1;				//定时器1开始计时
	ET1 = 1;				//使能定时器1中断
	EA =1 ;
}





void main()
{
	System_Init();
	Timer1_Init();
	while(1)
	{
		Led_Proc();
		Key_Proc();
		Seg_Proc();
		
	}
}