/* 头文件声明区 */
#include <REGX52.H>//单片机寄存器专用头文件
#include <Key.h>//按键底层驱动专用头文件
#include <Seg.h>//数码管底层驱动专用头文件

/* 变量声明区 */
unsigned char Key_Val,Key_Down,Key_Old;//按键专用变量
unsigned char Key_Slow_Down;//按键减速专用变量
unsigned char Seg_Buf[6] = {10,10,10,10,10,10};//数码管显示数据存放数组
unsigned char Seg_Buf_Point[6] = {0,1,0,1,0,1};//显示小数点
unsigned char Seg_Pos;//数码管扫描专用变量
unsigned int Seg_Slow_Down;//数码管减速专用变量
unsigned char Alarm_Set_Buf[3] = {0,0,0};//闹钟设置数组
unsigned char Alarm_Index;//闹钟设置数组索引
unsigned char Time[3] = {23,59,55};//时钟设置数组
unsigned char Time_Index ;//时钟设置数组索引
unsigned char Hour=23,Minute=59,Second=55;//时分秒
unsigned char Clock_Mode;//0--时钟显示	1--时钟设置	2--闹钟设置
unsigned int Timer250;//0.5秒闪烁
bit Flag500;//0.5秒闪烁标志位
unsigned int Timer1000;//一秒计时
bit TimeFlag = 0;//时间到标志位
/* 键盘处理函数 */
void Key_Proc()
{
	if(Key_Slow_Down) return;
	Key_Slow_Down = 1;//键盘减速程序

	Key_Val = Key_Read();//实时读取键码值
	Key_Down = Key_Val & (Key_Old ^ Key_Val);//捕捉按键下降沿
	Key_Old = Key_Val;//辅助扫描变量
	if(Key_Down) TimeFlag = 0;

	switch(Key_Down)
	{
		case 1:
			if (Clock_Mode == 0) 
			{
				Time[0] = Hour;
				Time[1] = Minute;
				Time[2] = Second;
				Clock_Mode = 1;
			}
			break;
		case 2:
			if (Clock_Mode == 0) Clock_Mode = 2;
			break;
		case 3:
			if(Clock_Mode == 1)
			{
				if( ++Time_Index == 3) Time_Index = 0;
			}
			if(Clock_Mode == 2)
			{
				if( ++Alarm_Index == 3) Alarm_Index = 0;
			}
			break;
		case 4:
			
			break;
		case 5:
			if(Clock_Mode == 1)
			{
				if(Time_Index == 0)
				{
					if(++ Time[Time_Index] == 24)	Time[Time_Index] = 0;
				}else
				{
					if(++ Time[Time_Index] == 60)	Time[Time_Index] = 0;
				}
				
			}
			if(Clock_Mode == 2)
			{
				if(Time_Index == 0)
				{
					if(++ Alarm_Set_Buf[Alarm_Index] == 24)	Alarm_Set_Buf[Alarm_Index] = 0;
				}else
				{
					if(++ Alarm_Set_Buf[Alarm_Index] == 60)	Alarm_Set_Buf[Alarm_Index] = 0;
				}
				
			}
			
			break;
		case 6:
			if(Clock_Mode == 1)
			{
				if(Time_Index == 0)
				{
					if(-- Time[Time_Index] == 255)	Time[Time_Index] = 23;
				}else
				{
					if(-- Time[Time_Index] == 255)	Time[Time_Index] = 59;
				}
				
			}
			if(Clock_Mode == 2)
			{
				if(Time_Index == 0)
				{
					if(-- Alarm_Set_Buf[Alarm_Index] == 255) Alarm_Set_Buf[Alarm_Index] = 23;
				}else
				{
					if(-- Alarm_Set_Buf[Alarm_Index] == 255)	Alarm_Set_Buf[Alarm_Index] = 59;
				}
				
			}
			
			break;
		case 7:
			if(Clock_Mode == 1)
			{
				Hour = Time[0];
				Minute = Time[1];
				Second = Time[2];
				Clock_Mode = 0;
			}
			if(Clock_Mode == 2)
			{
				Clock_Mode = 0;
			}
			break;
		case 8:
			if(Clock_Mode == 1)
			{
				Clock_Mode = 0;
				Time[0] = Hour;
				Time[1] = Minute;
				Time[2] = Second;
			}
			if(Clock_Mode == 2)
			{
				Clock_Mode = 0;
				Alarm_Set_Buf[0] = 0;
				Alarm_Set_Buf[1] = 0;
				Alarm_Set_Buf[2] = 0;
			}
			break;
	}
}

/* 信息处理函数 */
void Seg_Proc()
{
	if(Seg_Slow_Down) return;
	Seg_Slow_Down = 1;//数码管减速程序

	if( Clock_Mode == 0 )
	{
		Seg_Buf[0] = Hour /10 %10;
		Seg_Buf[1] = Hour %10;
		Seg_Buf[2] = Minute /10 %10;
		Seg_Buf[3] = Minute %10;
		Seg_Buf[4] = Second /10 %10;
		Seg_Buf[5] = Second %10;
	}
	if( Clock_Mode == 1 )
	{
		if( Flag500 == 0)
		{
			Seg_Buf[2*Time_Index] = 10;
			Seg_Buf[1+2*Time_Index] = 10;
		}else
		{
		Seg_Buf[0] = Time[0] /10 %10;
		Seg_Buf[1] = Time[0] %10;
		Seg_Buf[2] = Time[1] /10 %10;
		Seg_Buf[3] = Time[1] %10;
		Seg_Buf[4] = Time[2] /10 %10;
		Seg_Buf[5] = Time[2] %10;
		}
	}
	if( Clock_Mode == 2 )
	{
		if( Flag500 == 0)
		{
			Seg_Buf[2*Alarm_Index] = 10;
			Seg_Buf[1+2*Alarm_Index] = 10;
		}else
		{
		Seg_Buf[0] = Alarm_Set_Buf[0] /10 %10;
		Seg_Buf[1] = Alarm_Set_Buf[0] %10;
		Seg_Buf[2] = Alarm_Set_Buf[1] /10 %10;
		Seg_Buf[3] = Alarm_Set_Buf[1] %10;
		Seg_Buf[4] = Alarm_Set_Buf[2] /10 %10;
		Seg_Buf[5] = Alarm_Set_Buf[2] %10;
		}
	}
}

/* 其他显示函数 */
void Led_Proc()
{
	if( (Hour == Alarm_Set_Buf[0]) && (Minute == Alarm_Set_Buf[1]) &&= (Second == Alarm_Set_Buf[2])) TimeFlag = 1;
	if(TimeFlag)
	{
		P2_3 = 0;
		if( Alarm_Set_Buf[0] < 12)
		{
			if( Flag500 == 0)
			{
				P1 ^= 0xF0;
			}
		}
		if( (Alarm_Set_Buf[0] > 12) || ((Alarm_Set_Buf[0] == 12) && (Alarm_Set_Buf[1]>= 0)) || ((Alarm_Set_Buf[0] == 12) && (Alarm_Set_Buf[1] == 0) && (Alarm_Set_Buf[2]>= 0)))
		{
				if( Flag500 == 0)
			{
				P1 ^= 0x0F;
			}
		}
	}else
	{
		P2_3 = 1;
		P1 = 0xFF;
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
	ET0 = 1;        //定时器0中断打开
	EA = 1;         //总中断打开
}

/* 定时器0中断服务函数 */
void Timer0Server() interrupt 1
{
 	TL0 = 0x18;		//设置定时初始值
	TH0 = 0xFC;		//设置定时初始值   
	if(++Key_Slow_Down == 10) Key_Slow_Down = 0;//键盘减速专用
	if(++Seg_Slow_Down == 50) Seg_Slow_Down = 0;//数码管减速专用
	if(++Seg_Pos == 6) Seg_Pos = 0;//数码管显示专用
	Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos],Seg_Buf_Point[Seg_Pos]);
	if(++Timer250 == 250) 
	{
		Timer250 = 0;
		Flag500 ^= 1;
	}
	if(++Timer1000 == 1000) 
	{
		Timer1000 = 0;
		if(++ Second == 60) 
		{
			Second = 0;
			if(++Minute == 60)
			{
				Minute = 0;
				if(++ Hour == 24) Hour = 0;
			}
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