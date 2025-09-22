/*头文件声明区*/
#include <STC15F2K60S2.H>
#include <Init.H>
#include <Led.H>
#include <Seg.H>
#include <Key.H>
#include <DS1302.H>
/*变量声明区*/
unsigned char Key_Val, Key_Down, Key_Up, Key_Old;
unsigned int Key_Slow_Down;
unsigned int Seg_Slow_Down;
unsigned char Seg_Buf[8]={10,10,10,10,10,10,10,10};
unsigned char Seg_Pos;
unsigned char Seg_Point[8] = {0,0,0,0,0,0,0,0};
unsigned char ucLed[8] = {0,0,0,0,0,0,0,0};
unsigned char ucRtc[3] = {0X23,0X59,0X55};

/*按键处理函数*/
void Key_Proc()
{
	if(Key_Slow_Down) return;
	Key_Slow_Down = 1;
	
	Key_Val = Key_Read();
	Key_Down = Key_Val & (Key_Val ^ Key_Old);
	Key_Up = ~ Key_Val & (Key_Val ^ Key_Old);
	Key_Old = Key_Val;
	
	
}

/*信息处理函数*/
void Seg_Proc()
{
	if(Seg_Slow_Down) return;
	Seg_Slow_Down = 1;
	
	/*信息读取区域*/
	Read_Rtc(ucRtc);
	
	/*数据处理区域*/
	Seg_Buf[0] = ucRtc[0] / 16;
	Seg_Buf[1] = ucRtc[0] % 16;
	Seg_Buf[2] = 11;
	Seg_Buf[3] = ucRtc[1] / 16;
	Seg_Buf[4] = ucRtc[1] % 16;
	Seg_Buf[5] = 11;
	Seg_Buf[6] = ucRtc[2] / 16;
	Seg_Buf[7] = ucRtc[2] % 16;

}
/*其他显示函数*/
void Led_Proc()
{
	
}
/*定时器0初始化函数*/
void Timer0Init(void)		//1毫秒@12.000MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x18;		//设置定时初始值
	TH0 = 0xFC;		//设置定时初始值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;
	EA = 1;
}

/*定时器0中断服务函数*/
void Timer0Server() interrupt 1
{
	if(++Key_Slow_Down == 10) Key_Slow_Down = 0;
	if(++Seg_Slow_Down == 100) Seg_Slow_Down = 0;
	if(++Seg_Pos == 8) Seg_Pos = 0;
	Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos],Seg_Point[Seg_Pos]);
	Led_Disp(Seg_Pos,ucLed[Seg_Pos]);
}
/*Main*/
void main()
{
	System_Init();
	Timer0Init();
	Set_Rtc(ucRtc);
	while(1)
	{
		Key_Proc();
		Seg_Proc();
		Led_Proc();
	}
}