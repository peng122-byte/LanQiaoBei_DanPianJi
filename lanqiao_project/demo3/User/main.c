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
unsigned int Seg_Slow_Down;//数码管减速专用变量
unsigned char Led_Mod ;//Led模式 0-模式一：从左到右 1-模式二：从右到左 2-模式三 3-模式四
unsigned char Led_Num,Led_Old;//Led编号
unsigned char Led_Num1,Led_Old1;
unsigned char Led_Num2,Led_Old2,Led_Num2s,Led_Old2s;
unsigned char Led_Num3,Led_Old3,Led_Num3s,Led_Old3s;
unsigned int Time[9] = {400,500,600,700,800,900,1000,1100,1200};//流转时间数组
unsigned char Time_Index;//流转时间数组索引
bit System_Flag ;//系统标志位
unsigned int Led_Set_Data [4] = {400,400,400,400};//各模式流转时间
unsigned int System_Tick;//流转时间计数
unsigned char Set_Mod;//模式变量
bit Setting;//模式标志位
unsigned char State[3] = {11,12,13};//S A -



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
		case 7:
			System_Flag ^= 1;
		break;
		case 6:
			if(System_Flag) 
			{
				Setting ^= 1;
				if(++Set_Mod == 3) 
				{
					Set_Mod = 0;
					Setting ^= 1;
				}
			}
		break;
		case 5:
			if(Setting == 1) 
			{
				if(Set_Mod == 1) 
				{
					
				}else
				{
					
				}
			}
		break;
		case 4:
			if(Setting == 1) 
			{
				if(Set_Mod == 1) 
				{
					
				}else
				{
					
				}
			}
		break;
			
	}
}



/* 信息处理函数 */
void Seg_Proc()
{
	if (Seg_Slow_Down) return;
	Seg_Slow_Down =1 ;
	if(Setting == 0)
	{
		Seg_Buf[0] = State[System_Flag];
		Seg_Buf[1] = Led_Mod +1;
		Seg_Buf[3] = Led_Set_Data[Led_Mod]/100;
		Seg_Buf[4] = Led_Set_Data[Led_Mod]%100 /10;
		Seg_Buf[5] = Led_Set_Data[Led_Mod]%100 %10;
	}
	
}




/* 其他显示函数 */
void Led_Proc()
{
	if(System_Tick == Led_Set_Data[Led_Mod])
	{
		System_Tick = 0;
		switch(Led_Mod)
		{
			case 0:
			if(Led_Num == 8) Led_Num = 0;
			Led_Disp(Led_Old,0);
			Led_Disp(Led_Num,1);
			Led_Old = Led_Num;
			Led_Num ++;
			if(Led_Old == 7) Led_Mod = 1;
			break;
			case 1:
			if(--Led_Num1 == 255) Led_Num1 = 7;
			Led_Disp(Led_Old1,0);
			Led_Disp(Led_Num1,1);
			Led_Old1 = Led_Num1;
			if(Led_Old1 == 0) Led_Mod = 2;

			break;
			case 2:
			if(Led_Num2 == 4) Led_Num2 = 0;
			Led_Num2s = 7 - Led_Num2;
			Led_Disp(Led_Old2s,0);
			Led_Disp(Led_Old2,0);
			Led_Disp(Led_Num2s,1);
			Led_Disp(Led_Num2,1);
			Led_Old2 = Led_Num2;
			Led_Old2s = Led_Num2s;
			Led_Num2 ++;
			if(Led_Old2 == 3) Led_Mod = 3;
			break;
			case 3:
			if(--Led_Num3 == 255) Led_Num3 = 3;
			Led_Num3s = 7 - Led_Num3;
			Led_Disp(Led_Old3s,0);
			Led_Disp(Led_Old3,0);
			Led_Disp(Led_Num3s,1);
			Led_Disp(Led_Num3,1);
			Led_Old3 = Led_Num3;
			Led_Old3s = Led_Num3s;
			if(Led_Old3 == 0) Led_Mod = 0;
			break;
			
		}
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
	if(++Seg_Slow_Down == 50) Seg_Slow_Down = 0;//数码管减速专用
	if(++Seg_Pos == 6) Seg_Pos = 0;//数码管显示专用
	Seg_Disp(Seg_Pos, Seg_Buf[Seg_Pos], Seg_Point[Seg_Pos]);
	if(System_Flag)
	{
		System_Tick ++;
		
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