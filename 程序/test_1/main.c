/*头文件声明区*/
#include <REGX52.H>
#include <intrins.h>
/*变量声明区*/
unsigned char Seg_Dula[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x00};
unsigned char Seg_Wela[]={0xfe,0xfd,0xfb,0xf7,0xef,0xdf};
unsigned char Seg_Pos;
unsigned char Seg_Buf[]={1,2,3,4,5,6};
unsigned char ucled=0xfe;
unsigned int xms;
unsigned char temp=0;
unsigned char Key_Val,Key_Down,Key_Up,Key_Old;
bit System_Flag;
/*延时函数*/
void Delay(xms)		//@12.000MHz
{
	unsigned char i, j;

	i = 2;
	j = 239;
	do
	{
		while (--j);
	} while (--i);
}

/*独立按键读取函数*/
unsigned char Key_Read0()
{
	
	if(P3_4==0) temp=1;
	if(P3_5==0) temp=2;
	if(P3_6==0) temp=3;
	if(P3_7==0) temp=4;
	return temp;
}

/*矩阵按键*/
unsigned char Key_Read1()
{
	P3_0=0;P3_1=1;P3_2=1;P3_3=1;
	if(P3_4==0) temp=1;
	if(P3_5==0) temp=2;
	if(P3_6==0) temp=3;
	if(P3_7==0) temp=4;
	P3_0=1;P3_1=0;P3_2=1;P3_3=1;
	if(P3_4==0) temp=5;
	if(P3_5==0) temp=6;
	if(P3_6==0) temp=7;
	if(P3_7==0) temp=8;
	P3_0=1;P3_1=1;P3_2=0;P3_3=1;
	if(P3_4==0) temp=9;
	if(P3_5==0) temp=10;
	if(P3_6==0) temp=11;
	if(P3_7==0) temp=12;
	P3_0=1;P3_1=1;P3_2=1;P3_3=0;
	if(P3_4==0) temp=13;
	if(P3_5==0) temp=14;
	if(P3_6==0) temp=15;
	if(P3_7==0) temp=16;
	return temp;
}

/*数码管显示函数*/
void Seg_Disp(unsigned char wela,dula)
{
	P0=0x00;
	P2_6=1;
	P2_6=0;
	
	P0=Seg_Wela[wela];
	P2_7=1;
	P2_7=0;
	
	P0=Seg_Dula[dula];
	P2_6=1;
	P2_6=0;
}
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
	TL0=0x18;
	TH0=0xFC;
	if(++Seg_Pos==6) Seg_Pos=0;
	Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos]);
}
/*Main*/
void main()
{
	Timer0Init();
	while(1)
	{
		Key_Val=Key_Read1();
		Key_Down=Key_Val&(Key_Val^Key_Old);
		Key_Up=~Key_Val&(Key_Val^Key_Old);
		Key_Old=Key_Val;
		
		
		if(System_Flag==1)
		{
			ucled=_crol_(ucled,1);
			P1=ucled;
			Delay(500);
		}
		switch(Key_Down)
		{
			case 1:
			System_Flag=1;
			break;
			case 2:
			System_Flag=0;
			break;
		}
		
	}
}