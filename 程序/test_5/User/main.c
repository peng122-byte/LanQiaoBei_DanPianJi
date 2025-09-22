/*头文件声明区*/
#include <REGX52.H>
#include "Key.h"
#include "Seg.h"
/*变量声明区*/
unsigned char Key_Val,Key_Down,Key_Up,Key_Old;
unsigned char Key_Slow_Down;
unsigned int Seg_Slow_Down;
unsigned char Seg_Pos;
unsigned char Seg_Buf[6]={1,2,3,4,5,6};
unsigned char Seg_Point [6] ={0,1,0,1,0,1};
/*按键处理函数*/
void Key_Proc()
{
	if(Key_Slow_Down) return;
	Key_Slow_Down=1;
	Key_Val= Key_Read();
	Key_Down= Key_Val & (Key_Old ^ Key_Val);
	Key_Up= ~ Key_Val & (Key_Old ^ Key_Val);
	Key_Old=Key_Val;
	switch (Key_Down)
	{
		
	}
}
/*数码管显示函数*/
void Seg_Proc()
{
	if(Seg_Slow_Down) return;
	Seg_Slow_Down=1;
}
/*其他显示函数*/
void Led_Proc()
{
	
}
/*定时器初始化函数*/
/*定时器初始化函数*/

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
/*定时器0中断服务函数*/
void Timer0Server() interrupt 1
{
	TL0 = 0x18;		//设置定时初始值
	TH0 = 0xFC;		//设置定时初始值
	if(++Key_Slow_Down==10) Key_Slow_Down=0;
	if(++Seg_Slow_Down==500) Seg_Slow_Down=0;
	if(++Seg_Pos==6) Seg_Pos=0;
	Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos],Seg_Point[Seg_Pos]);
}
/*Main*/
void main()
{
	Timer0Init();
	while(1)
	{
		Key_Proc;
		Seg_Proc;
		Led_Proc;
	}
}
