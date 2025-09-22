/*头文件声明区*/
#include <REGX52.H>
#include "Key.h"
#include "Seg.h"

/*变量声明区*/
unsigned char Key_Val,Key_Old,Key_Down,Key_Up;
unsigned char Key_Slow_Down;
unsigned int Seg_Slow_Down;
unsigned char Seg_Pos;
unsigned char Seg_Buf[6]={1,2,3,4,5,6};
unsigned char Seg_Disp_Mode;//0-显示 1-时钟设置 2-闹钟设置
unsigned char Clock_Disp[3]={23,59,55};
unsigned int Timer_1000Ms;
unsigned char Seg_Point [6] ={0,1,0,1,0,1};
unsigned char Clock_Set [3];
unsigned char Clock_Set_Index;
unsigned int Timer_500ms;
bit Seg_Flag;
unsigned char Alarm[]={0,0,0};
unsigned char Alarm_Set[3];
bit Alarm_Flag =1;
unsigned char Led;
bit Alarm_Enable_Flag;

/*按键处理函数*/
void Key_Proc()
{
	if(Key_Slow_Down) return;
	Key_Slow_Down=1;
	Key_Val= Key_Read();
	Key_Down= Key_Val & (Key_Old ^ Key_Val);
	Key_Up= ~ Key_Val & (Key_Old ^ Key_Val);
	Key_Old=Key_Val;
	if(Key_Down !=0)
	{
		Alarm_Enable_Flag =0;
	}
	
	switch (Key_Down)
	{
		case 1:
			Seg_Disp_Mode=0;
			Clock_Set[0]= Clock_Disp[0];
			Clock_Set[1]= Clock_Disp[1];
			Clock_Set[2]= Clock_Disp[2];
			Seg_Disp_Mode=1;
			break;
		case 3:
			if(Seg_Disp_Mode==1)
			{
				if(++Clock_Set_Index==3) Clock_Set_Index=0;
			}
			
			break;
		case 4:
			Alarm_Flag ^=1;
			break;
		case 5:
			if(Seg_Disp_Mode==1)
			{ 
				Clock_Set[Clock_Set_Index]++;
				if(Clock_Set[Clock_Set_Index] == (Clock_Set_Index==0?24:60))
				{
					Clock_Set[Clock_Set_Index]=0;
				}
			}
			if(Seg_Disp_Mode==2)
			{
				Alarm_Set[Clock_Set_Index]++;
				if(Alarm_Set[Clock_Set_Index] == (Clock_Set_Index==0?24:60))
				{
					Alarm_Set[Clock_Set_Index]=0;
				}
			}
			
			break;
		case 6:
			if(Seg_Disp_Mode==1)
			{
				Clock_Set[Clock_Set_Index]--;
				if(Clock_Set[Clock_Set_Index] == 255)
				{
					Clock_Set[Clock_Set_Index]=(Clock_Set_Index==0?23:59);
				}
			}
			if(Seg_Disp_Mode==2)
			{
				Alarm_Set[Clock_Set_Index]--;
				if(Alarm_Set[Clock_Set_Index] == 255)
				{
					Alarm_Set[Clock_Set_Index]=(Clock_Set_Index==0?23:59);
				}
			}
			break;
		case 7:
			if(Seg_Disp_Mode==1)
			{
				Clock_Disp[0]= Clock_Set[0];
				Clock_Disp[1]= Clock_Set[1];
				Clock_Disp[2]= Clock_Set[2];
			}
			if(Seg_Disp_Mode==2)
			{
				Clock_Disp[0]= Alarm_Set[0];
				Clock_Disp[1]= Alarm_Set[1];
				Clock_Disp[2]= Alarm_Set[2];
			}
			Seg_Disp_Mode=0;
			break;
		case 8:
			Seg_Disp_Mode=0;
			break;
		case 2:
			Seg_Disp_Mode=0;
			Alarm_Set[0]= Alarm[0];
			Alarm_Set[1]= Alarm[1];
			Alarm_Set[2]= Alarm[2];
			Seg_Disp_Mode=2;
	}
		
}


/*数码管处理函数*/
void Seg_Proc()
{
	if(Seg_Slow_Down) return;
	Seg_Slow_Down=1;
	switch(Seg_Disp_Mode)
	{
		case 0:
			Seg_Buf[0]=Clock_Disp [0]/10%10;
			Seg_Buf[1]=Clock_Disp [0]%10;
			Seg_Buf[2]=Clock_Disp [1]/10%10;
			Seg_Buf[3]=Clock_Disp [1]%10;
			Seg_Buf[4]=Clock_Disp [2]/10%10;
			Seg_Buf[5]=Clock_Disp [2]%10;
			break;
		case 1:
			Seg_Buf[0]=Clock_Set [0]/10%10;
			Seg_Buf[1]=Clock_Set [0]%10;
			Seg_Buf[2]=Clock_Set [1]/10%10;
			Seg_Buf[3]=Clock_Set [1]%10;
			Seg_Buf[4]=Clock_Set [2]/10%10;
			Seg_Buf[5]=Clock_Set [2]%10;
			/*switch(Clock_Set_Index)
			{
				case 0:
					Seg_Buf[0] = Seg_Flag?Clock_Set[0]/10%10:10;
					Seg_Buf[1]=Seg_Flag?Clock_Set [0]%10:10;
					break;
				case 1:
					Seg_Buf[2] = Seg_Flag?Clock_Set[1]/10%10:10;
					Seg_Buf[3]=Seg_Flag?Clock_Set [1]%10:10;
					break;
				case 2:
					Seg_Buf[4] = Seg_Flag?Clock_Set[2]/10%10:10;
					Seg_Buf[5]=Seg_Flag?Clock_Set [2]%10:10;
					break;
				
			}*/
			Seg_Buf[0+Clock_Set_Index*2] = Seg_Flag?Clock_Set[Clock_Set_Index]/10%10:10;
			Seg_Buf[1+Clock_Set_Index*2]=Seg_Flag?Clock_Set [Clock_Set_Index]%10:10;
			break;
		case 2:
			Seg_Buf[0]=Alarm_Set [0]/10%10;
			Seg_Buf[1]=Alarm_Set [0]%10;
			Seg_Buf[2]=Alarm_Set [1]/10%10;
			Seg_Buf[3]=Alarm_Set [1]%10;
			Seg_Buf[4]=Alarm_Set [2]/10%10;
			Seg_Buf[5]=Alarm_Set [2]%10;
			Seg_Buf[0+Clock_Set_Index*2] = Seg_Flag?Alarm_Set[Clock_Set_Index]/10%10:10;
			Seg_Buf[1+Clock_Set_Index*2]=Seg_Flag?Alarm_Set [Clock_Set_Index]%10:10;
			break;
		
	}
}


/*其他显示函数*/
void Led_Proc()
{
	if(Alarm_Flag == 1)
	{
		
		if((Clock_Disp[0]==Alarm[0])&&(Clock_Disp[1] == Alarm[1])&&(Clock_Disp[2]==Alarm[2]))Alarm_Enable_Flag = 1;
			if(Alarm_Enable_Flag == 1)
			{
				P2_3 =0;
				P1= Led;
			}
		
		}
	else
	{
		P2_3 = 1;
		P1 = 0xff;
	}
	
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
	TL0 = 0x18;		//设置定时初始值
	TH0 = 0xFC;		//设置定时初始值
	if(++Key_Slow_Down==10) Key_Slow_Down=0;
	if(++Seg_Slow_Down==500) Seg_Slow_Down=0;
	if(++Seg_Pos==6) Seg_Pos=0;
	Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos],Seg_Point[Seg_Pos]);
	if (++Timer_1000Ms==1000)
	{
		Timer_1000Ms = 0;
		Clock_Disp[2]++;
		if(Clock_Disp[2] == 60)
		{
			Clock_Disp[2]=0;
			Clock_Disp[1]++;
			if(Clock_Disp[1]==60)
			{
				Clock_Disp[1]=0;
				Clock_Disp[0]++;
				if(Clock_Disp[0]==24) Clock_Disp[0]=0;
			}
		}
	}
	if(++Timer_500ms==500)
	{
		Timer_500ms=0;
		Seg_Flag^=1;
		if(Clock_Disp[0]>=12)
		{	
			Led ^= 0xf0;
		}
		else
		{
			Led ^= 0x0f;
		}
	}
}

/*Main*/
void main()
{
	Timer0Init();
	while(1)
	{
		Key_Proc();
		Seg_Proc();
		Led_Proc();
	}
}

