/* 头文件声明区 */
#include <STC15F2K60S2.H>//单片机寄存器专用头文件
#include <Init.h>//初始化底层驱动专用头文件
#include <Led.h>//Led底层驱动专用头文件
#include <Key.h>//按键底层驱动专用头文件
#include <Seg.h>//数码管底层驱动专用头文件
#include "onewire.h"

/* 变量声明区 */
unsigned char Key_Val,Key_Down,Key_Old,Key_Up;//按键专用变量
unsigned char Key_Slow_Down;//按键减速专用变量
unsigned char Seg_Buf[8] = {10,10,10,10,10,10,10,10};//数码管显示数据存放数组
unsigned char Seg_Point[8] = {0,0,0,0,0,0,0,0};//数码管小数点数据存放数组
unsigned char Seg_Pos;//数码管扫描专用变量
unsigned int Seg_Slow_Down;//数码管减速专用变量
unsigned char ucLed[8] = {0,0,0,0,0,0,0,0};//Led显示数据存放数组
float t;
unsigned char Display_Mode;//显示模式
bit Mode1_Flag;//模式一闪烁标志位
unsigned int Tick;//计时变量
unsigned char Set_Index;//参数设置索引
unsigned char TMAX = 30;//最大参数
unsigned char TMIN = 20;//最小参数
unsigned char Led_Level[8] = {0,0,0,0,0,0,0,0};//每个LED独立的亮度等级(0-5)
unsigned char Led_PWM_Counter[8] = {0,0,0,0,0,0,0,0};//每个LED独立的PWM计数器(0-4循环)
unsigned char Error;//参数错误

/* 键盘处理函数 */
void Key_Proc()
{
	if(Key_Slow_Down) return;
	Key_Slow_Down = 1;//键盘减速程序

	Key_Val = Key_Read();//实时读取键码值
	Key_Down = Key_Val & (Key_Old ^ Key_Val);//捕捉按键下降沿
	Key_Up = ~Key_Val & (Key_Old ^ Key_Val);//捕捉按键上降沿
	Key_Old = Key_Val;//辅助扫描变量
	switch(Key_Down)
	{
		case 12:
			Display_Mode ++;
			if(Display_Mode == 2) 
			{
				if(TMAX > TMIN)
				{
					Display_Mode = 0;
					Error = 0;
				}else
				{
					Display_Mode = 0;
					TMAX = 30;
					TMIN = 20;
					Error = 1;
				}
			}
		break;
		case 13:
			if(Display_Mode == 1)
			{
				Set_Index ++;
				if(Set_Index == 2) Set_Index = 0;
			}
		break;
		case 14:
			if(Display_Mode == 1)
			{
				switch(Set_Index)
				{
					case 0:
						TMAX ++;
					if(TMAX >= 70) TMAX = 70;
					break;
					case 1:
						TMIN ++;
					if(TMIN >= 70) TMIN = 70;
					break;
				}
			}
		break;
		case 15:
			if(Display_Mode == 1)
			{
				switch(Set_Index)
				{
					case 0:
						TMAX --;
					if(TMAX <= 10) TMAX = 10;
					break;
					case 1:
						TMIN --;
					if(TMIN <= 10) TMIN = 10;
					break;
				}
			}
		break;
		case 16:
			TMIN = 20;
			TMAX = 30;
			Display_Mode = 0;
		break;
	}

}

/* 信息处理函数 */
void Seg_Proc()
{
	if(Seg_Slow_Down) return;
	Seg_Slow_Down = 1;//数码管减速程序

	t = read_t();



	switch(Display_Mode)
	{
		case 0:
			Seg_Buf[0] = 11;
			Seg_Buf[1] = 10;
			Seg_Buf[2] = 10;
			Seg_Buf[3] = 10;
			Seg_Buf[4] = (unsigned char)t / 10 % 10;
			Seg_Buf[5] = (unsigned char)t % 10;
			Seg_Point[5] = 1;
			Seg_Buf[6] = (unsigned int)(t * 10) % 10;
			Seg_Buf[7] = 11;
		break;
		case 1:
			Seg_Buf[0] = 12;
			Seg_Buf[1] = 10;
			Seg_Buf[2] = 10;
			Seg_Buf[3] = TMAX /10;
			Seg_Buf[4] = TMAX %10;
			Seg_Buf[5] = 13;
			Seg_Point[5] = 0;
			Seg_Buf[6] = TMIN /10;
			Seg_Buf[7] = TMIN %10;
			if(Mode1_Flag)
			{
					if(Set_Index == 0)
				{
					Seg_Buf[3] = 10;
					Seg_Buf[4] = 10;
				}
				if(Set_Index == 1)
				{
					Seg_Buf[6] = 10;
					Seg_Buf[7] = 10;
				}
			}


		break;
	}
}

/* 其他显示函数 */
void Led_Proc()
{
//	if((unsigned char) t > TMAX)
//	{
//		Led_Level[0] = 1;  // LED1亮度等级1 (20%占空比 - 超温微亮提示)
//		ucLed[0] = 1;
//	}else
//	{
//		ucLed[0] = 0;
//	}
//	if((unsigned char) t < TMIN)
//	{
//		Led_Level[2] = 4;  // LED3亮度等级4 (80%占空比 - 低温高亮警告)
//		ucLed[2] = 1;
//	}else
//	{
//		ucLed[2] = 0;
//	}
//	if(((unsigned char) t <= TMAX) && ((unsigned char) t >= TMIN))
//	{
//		Led_Level[1] = 2;  // LED2亮度等级2 (40%占空比 - 正常中等亮度)
//		ucLed[1] = 1;
//	}else
//	{
//		ucLed[1] = 0;
//	}
//	if(Error)
//	{
//		Led_Level[3] = 5;  // LED4亮度等级5 (100%全亮 - 错误刺眼告警)
//		ucLed[3] = 1;
//	}else
//	{
//		ucLed[3] = 0;
//	}
}

/* 定时器0中断初始化函数 */
void Timer0Init(void)		//1毫秒@12.000MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x18;		//设置定时初始值
	TH0 = 0xFC;		//设置定时初始值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;    //定时器中断0打开
	EA = 1;     //总中断打开
}

/* 定时器0中断服务函数 */
void Timer0Server() interrupt 1
{  
	if(++Key_Slow_Down == 10) Key_Slow_Down = 0;//键盘减速专用
	if(++Seg_Slow_Down == 50) Seg_Slow_Down = 0;//数码管减速专用
	if(++Seg_Pos == 8) Seg_Pos = 0;//数码管显示专用
	Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos],Seg_Point[Seg_Pos]);
	
	if(Display_Mode == 1)
	{
			if(++Tick == 250)
		{
			Tick = 0;
			Mode1_Flag ^= 1;
		}
	}

//	// LED独立PWM控制 - 同时处理所有LED(静态显示模式)
//	// PWM周期5ms, 频率200Hz, 5级亮度调节(0-5)
//	{
//		unsigned char i;
//		// 更新所有LED的PWM计数器 (0-4循环，周期5ms)
//		for(i = 0; i < 8; i++)
//		{
//			if(++Led_PWM_Counter[i] == 5)
//				Led_PWM_Counter[i] = 0;

//			// 根据PWM占空比决定该LED状态
//			// Led_Level范围0-5: 0=关闭, 1=20%, 2=40%, 3=60%, 4=80%, 5=100%
//			if(Led_PWM_Counter[i] < Led_Level[i] && ucLed[i])
//				Led_Disp(i, 1);
//			else
//				Led_Disp(i, 0);
//		}
//	}
}
void Delay750ms()		//@12.000MHz
{
	unsigned char i, j, k;
	i = 35;
	j = 51;
	k = 182;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

/* Main */
void main()
{
	read_t();
	Delay750ms();
	System_Init();
	Timer0Init();
	while (1)
	{
		Key_Proc();
		Seg_Proc();
		Led_Proc();
	}
}