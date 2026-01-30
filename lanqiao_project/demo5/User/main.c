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
unsigned int	Seg_Slow_Down;//数码管减速专用变量
unsigned char	ucLed[8] = {0,0,0,0,0,0,0,0};//Led显示数组
unsigned char	Led_Pos;//Led扫描专用变量
unsigned char Seg_Dis_Mode;//数码管显示模式 0-温度采集界面 1-数据显示界面 2-参数设置界面
unsigned char Temperature_Input[3] = {13,13,13};//温度输入数组
unsigned char Temperature_Input_Point[3] = {0,0,0};//小数点输入数组
bit Num_Input;//数字输入完成标志位
bit Point_Input;//小数点输入完成标志位
unsigned int	Temperature_Input_Data;//温度输入数据
unsigned char Temperature_Input_Index ;//温度输入数组索引
unsigned int	Time250; 
bit Flag_0;//模式0闪烁标志位
unsigned char Temperature_Real [3] ={0,0,0};//实际温度数据
unsigned char	TMAX;//温度上限参数
unsigned char TMIN;//温度下限参数
unsigned char Temperature_Setting[4]	={3,0,2,0};//温度参数设置数组
unsigned char Temperature_Setting_Index;//温度参数设置数组索引
bit Flag_2;//模式2闪烁标志位
unsigned char Point_Wei;//小数点所在位数
bit Mode_Switch;//模式切换标志位
/* 键盘处理函数 */
void Key_Proc()
{
	unsigned char i;
	unsigned char j;
	if (Key_Slow_Down) return;
	Key_Slow_Down = 1;//按键减速程序
	
	Key_Val = Key_Read();//读取按下的键值
	Key_Down = Key_Val & (Key_Val ^ Key_Old);//下降沿
	Key_Up = Key_Old & (Key_Val ^ Key_Old);//上升沿
	
	Key_Old = Key_Val;//辅助扫描
	
	if(Seg_Dis_Mode == 0)
	{
		
		if((Key_Down > 0) && (Key_Down <= 10)	)
		{
			if(Temperature_Input_Index < 3)
			{
				Temperature_Input[Temperature_Input_Index] = Key_Down - 1;
				Num_Input = 1;
				Temperature_Input_Index	++;
			}
		}
		if((Key_Down == 11)	&&	(Num_Input ==	1) && (Point_Input == 0))
		{
			if(Temperature_Input_Index > 0)
			{
				Num_Input = 0;
				Point_Input = 1;
				Temperature_Input_Point[Temperature_Input_Index	- 1] = 1;

			}
		}
	}
	if(Key_Down == 16)
	{
		if(Seg_Dis_Mode == 0)
		{
			for(i=0;i<3;i ++)
			{
				if(Temperature_Input_Point[i]	== 1)	Point_Wei = i;
				if(Point_Input == 0) Point_Wei = 2;
				
			}
			switch(Point_Wei)
			{
				case 0:
					Temperature_Real [1] = 0;
					Temperature_Real [2] = Temperature_Input [0];
					if(Temperature_Input [1] >= 5)
					{
						Temperature_Real [2] ++;
						if(Temperature_Real [2] == 10)
						{
							Temperature_Real [2] = 0;
							Temperature_Real [1] ++;
						}
					}
				break;
				case 1:
					Temperature_Real [1] = Temperature_Input [0];
					Temperature_Real [2] = Temperature_Input [1];
					if(Temperature_Input [2] >= 5)
						{
							Temperature_Real [2] ++;
							if(Temperature_Real [2] == 10)
							{
								Temperature_Real [2] = 0;
								Temperature_Real [1] ++;
								if(Temperature_Real [1] == 10)
								{
									Temperature_Real [1] = 0;
									Temperature_Real [0] ++;
								}
							}
						}
				break;
				case 2:
					Temperature_Input_Index = 0;  //重置！
					Num_Input = 0;
					Point_Input = 0;
					for(j=0;j<3;j++)
					{
						Temperature_Real [j] = 0;
					}
					for(i=0;i<3;i++)
					{
						Temperature_Input_Point[i] = 0;
						Temperature_Input[i] = 13;
					}
				break;
			}
			Temperature_Input_Data = Temperature_Real [0]*100 + Temperature_Real [1]*10 + Temperature_Real [2];
			if((Temperature_Input_Data >= 0) && (Temperature_Input_Data <= 85) && (Num_Input == 1))
			{
				Seg_Dis_Mode = 1;
			}else
			{
				Temperature_Input_Index = 0;  //重置！
				Num_Input = 0;
				Point_Input = 0;
				for(j=0;j<3;j++)
				{
					Temperature_Real [j] = 0;
				}
				for(i=0;i<3;i++)
				{
					Temperature_Input_Point[i] = 0;
					Temperature_Input[i] = 13;
				}
			}

		}else
		{
			Seg_Dis_Mode = 0;
		}
	}
	if(Key_Down == 12)
	{
		if(Seg_Dis_Mode == 1)
		{
			Seg_Dis_Mode = 2;
		}else if(Seg_Dis_Mode == 2)
		{
			Seg_Dis_Mode = 1;
		}
	}
}



/* 信息处理函数 */
void Seg_Proc()
{
	if (Seg_Slow_Down) return;
	Seg_Slow_Down =1 ;
	switch (Seg_Dis_Mode)
	{
		case 0:
			Seg_Buf[0] = 12;
			Seg_Buf[1] = 10;
			Seg_Buf[2] = 10;
			Seg_Buf[3] = Temperature_Input[0];
			Seg_Point[3] = Temperature_Input_Point[0];
			Seg_Buf[4] = Temperature_Input[1];
			Seg_Point[4] = Temperature_Input_Point[1];
			Seg_Buf[5] = Temperature_Input[2];
			
			if(Flag_0)
			{
				if(Temperature_Input_Index < 2)
				{
					Seg_Buf[Temperature_Input_Index + 3] = 10;
				}
				if(Temperature_Input_Index == 2)	Seg_Buf[5] = 10;
			}
		break;
		case 1:
			Seg_Buf[0] = 14;
			Seg_Buf[1] = 10;
			Seg_Buf[2] = 10;
			Seg_Buf[3] = 10;
			Seg_Point[3] = 0;
			Seg_Point[4] = 0;
			Seg_Buf[4] = Temperature_Real [1] ;
			Seg_Buf[5] = Temperature_Real [2] ;
		break;
		case 2:
			Seg_Buf[0] = 15;
			Seg_Buf[1] = 10;
			Seg_Buf[2] = Temperature_Setting[0] ;
			Seg_Buf[3] = Temperature_Setting[1] ;
			Seg_Buf[4] = Temperature_Setting[2] ;
			Seg_Buf[5] = Temperature_Setting[3] ;
		if(Flag_2)
			{
				Seg_Buf[Temperature_Setting_Index + 2] = 10;
				Seg_Buf[Temperature_Setting_Index + 3] = 10;
			}
		break;
	}
}
/* 其他显示函数 */
void Led_Proc()
{
	
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
	if(++Led_Pos == 8) Led_Pos = 0;//Led显示专用
	Led_Disp(Led_Pos,ucLed[Led_Pos]);
	Seg_Disp(Seg_Pos, Seg_Buf[Seg_Pos], Seg_Point[Seg_Pos]);
	if(++Time250 ==250)
	{
		Time250 = 0;
		if(Seg_Dis_Mode == 0)	Flag_0 ^= 1;
		if(Seg_Dis_Mode == 2) Flag_2 ^= 1;
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