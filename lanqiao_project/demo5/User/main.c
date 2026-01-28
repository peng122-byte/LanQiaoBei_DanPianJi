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

/* 键盘处理函数 */
void Key_Proc()
{
	if (Key_Slow_Down) return;
	Key_Slow_Down = 1;//按键减速程序
	
	Key_Val = Key_Read();//读取按下的键值
	Key_Down = Key_Val & (Key_Val ^ Key_Old);//下降沿
	Key_Up = Key_Old & (Key_Val ^ Key_Old);//上升沿
	
	Key_Old = Key_Val;//辅助扫描
	
	
}



/* 信息处理函数 */
void Seg_Proc()
{
	if (Seg_Slow_Down) return;
	Seg_Slow_Down =1 ;
	
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
	if(++Led_Pos == 8) Seg_Pos = 0;//Led显示专用
	Led_Disp(Led_Pos,ucLed[Led_Pos]);
	Seg_Disp(Seg_Pos, Seg_Buf[Seg_Pos], Seg_Point[Seg_Pos]);
	
	
	
	
		

	
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