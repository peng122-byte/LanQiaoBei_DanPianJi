/* 头文件声明区 */
#include <REGX52.H>//单片机寄存器专用头文件
#include <Key.h>//按键底层驱动专用头文件
#include <Seg.h>//数码管底层驱动专用头文件
#include <intrins.h>
#include <Led.h>
/* 变量声明区 */
unsigned char Key_Val,Key_Down,Key_Old,Key_Up;//按键专用变量
unsigned char Key_Slow_Down;//按键减速专用变量
unsigned char Seg_Buf[6] = {10,10,10,10,10,10};//数码管显示数据存放数组
unsigned char Seg_Point[6] = {0,0,0,0,0,0};//数码管小数点数据存放数组
unsigned char Seg_Pos;//数码管扫描专用变量
unsigned char Seg_Point_Pos;//数码管小数点扫描专用变量
unsigned int Seg_Slow_Down;//数码管减速专用变量
unsigned char Seg_Disp_Mode;//数码管显示模式变量 0-运行状态界面 1-流转时间设置界面 
unsigned int Time;//1000ms计时
unsigned char Time_Counter = 30 ;//倒计时计数
unsigned char Seg_Set_Buf[6] = {10,10,10,10,10,10};//设置界面数据存放数组
bit Sys_Flag ;//系统标志位
unsigned char Time_Buf[3]={30,60,15};//设置时钟区间
unsigned char Time_Buf_Index ;//设置时钟区间索引
unsigned int Time500;//模式二数码管闪烁计时
bit Seg_Flag;//闪烁标志位
/* 键盘处理函数 */
void Key_Proc()
{
	if (Key_Slow_Down) return;
	Key_Slow_Down = 1;//按键减速程序
	
	Key_Val = Key_Read();//读取按下的键值
	Key_Down = Key_Val & (Key_Val ^ Key_Old);//下降沿
	Key_Up = Key_Old & (Key_Val ^ Key_Old);//上升沿
	
	Key_Old = Key_Val;//辅助扫描
	switch(Key_Down)
	{
		case 1 : 
			if(Seg_Disp_Mode ==0)Sys_Flag = 1 ;
		break;
		case 2 : 
			if(Seg_Disp_Mode ==0)
			{	
				Time_Counter = Time_Buf[Time_Buf_Index];
				Sys_Flag = 0 ;
			}
		break;
		case 3 : 
			if(Seg_Disp_Mode == 1 )
			{
				
				Time_Counter = Time_Buf[Time_Buf_Index];
			}
			Seg_Disp_Mode ^=1 ;
		break;
		case 4 : 
			if(Seg_Disp_Mode == 1) 
			{
				if(++Time_Buf_Index == 3) Time_Buf_Index = 0;
			}
			
			
		break;
	}
}

/* 信息处理函数 */
void Seg_Proc()
{
	if (Seg_Slow_Down) return;
	Seg_Slow_Down =1 ;
	
	Seg_Buf[0] = Seg_Disp_Mode + 1;
	if(Seg_Disp_Mode == 0)
	{
		
		Seg_Buf[4] = Time_Counter /10 %10;
		Seg_Buf[5] = Time_Counter %10;
	}else
	{
		Seg_Set_Buf[4] = Time_Buf[Time_Buf_Index] /10 %10;
		Seg_Set_Buf[5] = Time_Buf[Time_Buf_Index]  %10;
		if(Seg_Flag == 0) 
		{
			Seg_Buf[4] = 10;
			Seg_Buf[5] = 10;
		}else
		{
			Seg_Buf[4] = Seg_Set_Buf[4] ;
			Seg_Buf[5] = Seg_Set_Buf[5] ;
		}

		
	}
}

/* 其他显示函数 */
void Led_Proc()
{
	if(Time_Counter == 0) 
	{
		P1 = 0x00;
		P2_3 = 0;
	}
	else
	{
		P1 = 0xff;
		P2_3 = 1;
	}
	
	
}

/* 定时器0中断初始化函数 */
void Timer0Init(void)		//1毫秒@12.000MHz
{
	TMOD &= 0xF0;		//设置定时器模式
	TMOD |= 0x01;		//设置定时器模式
	TL0 = 0x18;		//设置定时初始值
	TH0 = 0xFC;		//设置定时初始值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;    //定时器0中断打开
	EA = 1;     //总中断打开
}

/* 定时器0中断服务函数 */
void Timer0Server() interrupt 1
{
 	TL0 = 0x18;		//设置定时初始值
	TH0 = 0xFC;		//设置定时初始值   
	if(++Key_Slow_Down == 10) Key_Slow_Down = 0;//键盘减速专用
	if(++Seg_Slow_Down == 20) Seg_Slow_Down = 0;//数码管减速专用
	if(++Time500 == 500) 
	{
		Time500 = 0;//模式2数码管闪烁
		Seg_Flag ^= 1;//闪烁状态转换
	}
	if(++Seg_Pos == 6) Seg_Pos = 0;//数码管显示专用
	if(++Seg_Point_Pos == 6) Seg_Point_Pos = 0;//数码管小数点显示专用
	Seg_Disp(Seg_Pos, Seg_Buf[Seg_Pos], Seg_Point[Seg_Point_Pos]);
	
	
	
	if (Sys_Flag == 1)
	{
		if(++Time == 1000)
		{
			Time = 0;
			Time_Counter --;
			if(Time_Counter ==255) Time_Counter = 0;
		}
		
	}
		

	
}

/* Main */
void main()
{
	Timer0Init();
	while (1)
	{
		Key_Proc();
		Seg_Proc();
		Led_Proc();
	}
}