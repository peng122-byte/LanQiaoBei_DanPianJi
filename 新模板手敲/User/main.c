#include <STC15F2K60S2.H>
#include "LED.h"
#include "init.h"
#include "Seg.h"
#include "Key.h"
#include "ds1302.h"
#include "onewire.h"
pdata unsigned char ucLed[8] = {1,0,1,0,1,0,1,0};
pdata unsigned char Seg_Buf[8] = {10,10,10,10,10,10,10,10};
idata unsigned char Seg_Pos = 0;
idata unsigned char Seg_Slow_Down;

idata unsigned char Key_Val,Key_Old,Key_Up,Key_Down;
idata unsigned char Key_Slow_Down;

//idata unsigned char Key_Down_Data , Key_Up_Data;//Key调试用

idata unsigned char ucRtc[3] = {11,12,13};
idata unsigned char Time_Slow_Down;

idata unsigned int Temperature_10x;
idata unsigned int Temperature_Slow_Down;


void Key_Proc()
{
	if(Key_Slow_Down < 10) return;
	Key_Slow_Down = 0;
	
	Key_Val = Key_Read();
	Key_Down = Key_Val & (Key_Val ^ Key_Old);
	Key_Up = ~ Key_Val & (Key_Val ^ Key_Old);
	Key_Old = Key_Val;
//	if(Key_Down != 0)
//		Key_Down_Data = Key_Down;
//	if(Key_Up != 0)
//		Key_Up_Data = Key_Up;
}


void Timer1_Isr(void) interrupt 3
{
	Seg_Slow_Down ++;
	Key_Slow_Down ++;
	Time_Slow_Down ++;
	Temperature_Slow_Down ++;
	Seg_Pos = (++Seg_Pos) % 8;
	if(Seg_Buf[Seg_Pos] > 20) 
	{
		Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos]- ',',1);
	}else
	{
		Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos],0);
	}
}


void Seg_Proc()
{
	if(Seg_Slow_Down < 20) return;
	Seg_Slow_Down = 0;
	
}

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
void Get_Time()
{
	if(Time_Slow_Down < 100) return;
	Time_Slow_Down = 0;
	Read_Rtc(ucRtc);
}

void Get_Temperature()
{
	if(Temperature_Slow_Down <300) return;
	Temperature_Slow_Down = 0;
	Temperature_10x = rd_temperature() *10;
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
	Set_Rtc(ucRtc);
	Timer1_Init();
	while(1)
	{
		Key_Proc();
		Led_Proc();
		Seg_Proc();
		Get_Time();
		Get_Temperature();
	}
}