/* 头文件声明区 */
#include <REGX52.H>//单片机寄存器专用头文件
#include <Key.h>//按键底层驱动专用头文件
#include <Seg.h>//数码管底层驱动专用头文件
#include <intrins.h>
#include <Led.h>
/* 变量声明区 */
unsigned char Key_Val,Key_Down,Key_Old,Key_Up;//按键专用变量
unsigned char Key_Slow_Down;//按键减速专用变量
unsigned char Seg_Buf[6] = {10,10,13,13,13,13};//数码管显示数据存放数组
unsigned char Seg_Point[6] = {0,0,0,0,0,0};//数码管小数点数据存放数组
unsigned char Seg_Pos;//数码管扫描专用变量
unsigned int Seg_Slow_Down;//数码管减速专用变量
unsigned char Disp_Mode = 0;//数码管显示模式：0-电压采集	1-数据显示	2-参数设置 3-计数统计
unsigned char Voltage_Input [4] ={13,13,13,13};//电压输入数组
unsigned char Voltage_Data [4] ={0,0,0,0};//电压采集数据数组
unsigned char Voltage_Input_Index;//电压数据输入索引
unsigned char Voltage_Data_Index;//电压数据索引
unsigned int Timer250;//500ms闪烁计时
bit Input_Flag;//闪烁标志位
unsigned char Voltage_Val;//输入电压值
unsigned char Key_Error_Count;//按键溢出变量

/* 键盘处理函数 */
void Key_Proc()
{
	unsigned char i;
	if (Key_Slow_Down) return;
	Key_Slow_Down = 1;//按键减速程序
	
	Key_Val = Key_Read();//读取按下的键值
	Key_Down = Key_Val & (Key_Val ^ Key_Old);//下降沿
	Key_Up = Key_Old & (Key_Val ^ Key_Old);//上升沿
	
	Key_Old = Key_Val;//辅助扫描
	
	
	if(Disp_Mode == 0)
	{
		if(Voltage_Input_Index < 4)  // 改为 < 4，防止越界
		{
			if((Key_Down >= 1) && (Key_Down <= 10))
			{
				Voltage_Val = Key_Down - 1;
				Voltage_Input[Voltage_Input_Index] = Voltage_Val;
				Voltage_Input_Index ++;
				Key_Error_Count = 0;
			}else
			{
			Key_Error_Count ++;
			}
		}

		// 按键11确认：独立判断，不受输入索引限制
		if((Key_Down == 11) && (Voltage_Input_Index == 4) )
		{
			for(i = 0;i<4;i++)
			{
				Voltage_Data[i] = Voltage_Input[i];
			}
			Disp_Mode = 1;
		}
	}
	
	
	if(Key_Down == 12)
	{
		if(++ Disp_Mode == 4) Disp_Mode = 0;
	}
	
	
	
	
	
	
	
	
	
	
}




/* 信息处理函数 */
void Seg_Proc()
{
	if (Seg_Slow_Down) return;
	Seg_Slow_Down =1 ;
	switch(Disp_Mode)
	{
		case 0:
			Seg_Buf[0] = 10;
			Seg_Buf[1] = 10;
			// 显示已输入的4位数据
			Seg_Buf[2] = Voltage_Input[0];
			Seg_Buf[3] = Voltage_Input[1];
			Seg_Buf[4] = Voltage_Input[2];
			Seg_Buf[5] = Voltage_Input[3];
			// 当前输入位闪烁（只在未输入满4位时闪烁）重要！！！
			if(Voltage_Input_Index < 4 && Input_Flag == 1)
			{
				Seg_Buf[Voltage_Input_Index + 2] = 10;
			}
		break;
		case 1:
			Seg_Buf[0] = 14;
			Seg_Buf[1] = 10;
			Seg_Buf[2] = 10;
			Seg_Buf[3] = Voltage_Data[0];
			Seg_Point[3] = 1;
			Seg_Buf[4] = Voltage_Data[1];
			Seg_Buf[5] = Voltage_Data[2];
			if(Voltage_Data[3] >= 5)	Seg_Buf[5] = Voltage_Data[2] + 1;
			
			
		break;
		case 2:
			
		break;
		case 3:
			
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
	Seg_Disp(Seg_Pos, Seg_Buf[Seg_Pos], Seg_Point[Seg_Pos]);
	if (++ Timer250 == 250)
	{
		Timer250 = 0;
		Input_Flag ^= 1;
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