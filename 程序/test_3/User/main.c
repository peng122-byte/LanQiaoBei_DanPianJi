/*头文件声明区*/
#include <REGX52.H>
#include "Key.h"
#include "Seg.h"

/*变量声明区*/
unsigned char Key_Slow_Down;//10ms
unsigned int Seg_Slow_Down;//500ms
unsigned char Key_Val,Key_Down,Key_Up,Key_Old;
unsigned char Seg_Pos;
unsigned char Seg_Buf[6]={10,10,10,10,10,10};
unsigned char Seg_Mode;//0-显示 1-设置
unsigned int Timer_1000ms;
unsigned char Timer_Count=30;
bit System_Flag;//系统标志位 0-暂停 1-开始
unsigned char Set_Dat[3] = {15,30,60};
unsigned char Set_Dat_Index=1;
unsigned int Timer_500ms;
bit Seg_Flag;
/*按键处理函数*/
void Key_Proc()
{
	if(Key_Slow_Down) return;
	Key_Slow_Down=1;
	
	Key_Val=Key_Read();
	Key_Down=Key_Val&(Key_Val^Key_Old);
	Key_Up=~Key_Val&(Key_Val^Key_Old);
	Key_Old=Key_Val;
	
	switch(Key_Down)
	{
		case 1:
			if (Seg_Mode==0) System_Flag =1;
			break;
		case 2:
			Timer_Count = Set_Dat[Set_Dat_Index];
			break;
		case 3:
			if(Seg_Mode==1) Timer_Count = Set_Dat[Set_Dat_Index];
			Seg_Mode ^=1;
			break;
		case 4:
			if(Seg_Mode==1)
			{
			if(++Set_Dat_Index==3) Set_Dat_Index=0;
			}
			break;
	}
		
	
	
	
}

/*信息处理函数*/
void Seg_Proc()
{
	if(Seg_Slow_Down) return;
	Key_Slow_Down=1;
	Seg_Buf[0]=Seg_Mode+1;
	if(Seg_Mode==0)
	{
		Seg_Buf[4]=Timer_Count /10 % 10;
		Seg_Buf[5]=Timer_Count % 10;
	}
	else
	{
		if(Seg_Flag ==1)
		{
			
		Seg_Buf[4]=Set_Dat[Set_Dat_Index] / 10 % 10;
		Seg_Buf[5]=Set_Dat[Set_Dat_Index] % 10;
		}
		else
		{
			Seg_Buf[4]=10;
			Seg_Buf[5]=10;
		}
	}
}

 /*其他显示函数*/
void Led_Proc()
{
	if(Timer_Count ==0)
	{
		P1=0x00;
		P2_3=0;
	}
	else
	{
		P1=0xff;
		P2_3=1;
	}
}


/*定时器0初始化函数*/
void Timer0Init(void)		//1毫秒@12.000MHz
{
	TMOD &= 0xF0;		//设置定时器模式
	TMOD |= 0x01;		//设置定时器模式
	TL0 = 0x18;		//设置定时初始值
	TH0 = 0xFC;		//设置定时初始值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0=1;
	EA=1;
}

/*定时器0中段服务函数*/
void Timer0Server() interrupt 1
{
	TL0 = 0x18;		//设置定时初始值
	TH0 = 0xFC;		//设置定时初始值
	if(++Key_Slow_Down==10) Key_Slow_Down=0;
	if (++Seg_Slow_Down==500) Seg_Slow_Down=0;
	if(++Seg_Pos==6) Seg_Pos=0;
	Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos]);
	if(System_Flag ==1)
	{
	if(++Timer_1000ms==1000)
	{
		Timer_1000ms=0;
		Timer_Count--;
		if(Timer_Count==255) Timer_Count = 0;
	}
	
	}
	if(++Timer_500ms==500)
	{
		Timer_500ms=0;
		Seg_Flag ^=1; 
	}
}

/*Main*/
void main()
{
	Timer0Init();
	while(1)
	{
		Key_Proc();
		Seg_Proc();
		Led_Proc();
	}
}
