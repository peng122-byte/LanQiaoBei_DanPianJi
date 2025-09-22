/*头文件声明区*/
#include <STC15F2K60S2.H>//单片机头文件
#include <string.h>
#include <Init.h>//初始化底层驱动
#include <Key.h>//按键底层驱动
#include <Seg.h>//数码管底层驱动
#include <Led.h>//Led底层驱动

/*变量声明区*/
unsigned char Ket_val,Key_Down,Key_UP,Key_Old;//按键变量
unsigned char Key_Slow_Down;//按键减速变量
unsigned char Seg_Buf[8]={10,10,10,10,10,10,10,10};//数码管显示存放数组
unsigned char Seg_Pos;//数码管扫描专用
idata unsigned int Seg_Slow_Down;//数码管减速
idata unsigned char ucLed[8] ={10,10,10,10,10,10,10,10};//Led显示数组  

/*键盘处理*/
void Key_Proc()
{
	if (Key_Slow_Down<10) return;
	Key_Slow_Down=0;//键盘减速程序
	Key_Val=Key_Read();//实时读取键码值
	Key_Down=Key_Val&(Key_Old^Key_Val);//捕捉下降
	Key_Up=~Key_Val&(Key_Old^Key_Val)	;//捕捉上升
	Key_Old=Key_Val;
	
}
/*信息处理函数*/
void Seg_Proc()
{
	if(Seg_Slow_Down<500) return;
	Seg_Slow_Down=0;
}
/*其他显示函数*/
void Led_Proc()
{
	
}
/*定时器0中断初始化函数*/
void Timer0Init(void)		//1毫秒@12.000MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x18;		//设置定时初始值
	TH0 = 0xFC;		//设置定时初始值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ETO = 1;		//定时器中断0打开
	EA = 1;			//总中断打开
}
/*定时器0中断服务函数*/
void Timer0Server() interupt 1
{
	Key_Slow_Down++;
	Seg_Slow_Down++;
	
	if(++Seg_Pos==8)Seg_Pos=0;//数码管显示专用
	//数码管显示处理
}