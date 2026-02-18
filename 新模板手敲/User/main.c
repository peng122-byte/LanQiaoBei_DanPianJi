#include <STC15F2K60S2.H>
#include "LED.h"
#include "init.h"
#include "Seg.h"
#include "Key.h"
#include "ds1302.h"
#include "onewire.h"
#include "iic.h"
#include "ultrasound.h"
pdata unsigned char ucLed[8] = {1,0,1,0,1,0,1,0};
pdata unsigned char Seg_Buf[8] = {10,10,10,10,10,10,10,10};
idata unsigned char Seg_Pos = 0;
idata unsigned char Seg_Slow_Down;

idata unsigned char Key_Val,Key_Old,Key_Up,Key_Down;
idata unsigned char Key_Slow_Down;

idata unsigned char ucRtc[3] = {11,12,13};
idata unsigned char Time_Slow_Down;

idata unsigned int Temperature_10x;
idata unsigned int Temperature_Slow_Down;

idata unsigned char AD_1_Data_10x, AD_3_Data_10x;
idata unsigned char AD_DA_Slow_Down;

pdata unsigned char EEPROM_Data_W[8] = {1,2,3,4,5,6,7,8};
pdata unsigned char EEPROM_Data_R[8] = {0,0,0,0,0,0,0,0};

idata unsigned char Distance;
idata unsigned char Distance_Slow_Down;

idata unsigned int Freq;
idata unsigned int Time_1s;

idata unsigned char pwm_period;//累加对比
idata unsigned char pwm_compare = 6;//比较值，也就是亮度


void Key_Proc()
{
	if(Key_Slow_Down < 10) return;
	Key_Slow_Down = 0;

	Key_Val = Key_Read();
	Key_Down = Key_Val & (Key_Val ^ Key_Old);
	Key_Up = ~ Key_Val & (Key_Val ^ Key_Old);
	Key_Old = Key_Val;
	if(Key_Down == 4)
		pwm_compare= (++pwm_compare)%10;
	
}


void Timer1_Isr(void) interrupt 3
{
	Seg_Slow_Down ++;
	Key_Slow_Down ++;
	Time_Slow_Down ++;
	Temperature_Slow_Down ++;
	AD_DA_Slow_Down ++;
	Distance_Slow_Down ++;
	Seg_Pos = (++Seg_Pos) % 8;
	if(Seg_Buf[Seg_Pos] > 20)
	{
		Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos]- ',',1);
	}else
	{
		Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos],0);
	}
	if(++Time_1s == 1000)
	{
		Time_1s = 0;
		Freq = TH0 << 8|TL0;
		TH0 = TL0 = 0;
	}
	//调光
	pwm_period = (++ pwm_period )%10;
	if(pwm_period<pwm_compare)
		Led_Disp(ucLed);//如果不调光，要放在Led_Disp
	else
		Led_Off();
	
	
	
}


void Seg_Proc()
{
	if(Seg_Slow_Down < 20) return;
	Seg_Slow_Down = 0;

}

void Led_Proc()
{
	ucLed[0] = 1;
	ucLed[1] = 0;
	ucLed[2] = 1;
	ucLed[3] = 0;
	ucLed[4] = 1;
	ucLed[5] = 0;
	ucLed[6] = 1;
	ucLed[7] = 0;
	//Led_Disp(ucLed);
}

void Get_Time()
{
	if(Time_Slow_Down < 100) return;
	Time_Slow_Down = 0;
	Read_Rtc(ucRtc);
}

void Get_Temperature()
{
	if(Temperature_Slow_Down <300) return;
	Temperature_Slow_Down = 0;
	Temperature_10x = rd_temperature() *10;

}

void AD_DA()
{
	if(AD_DA_Slow_Down < 120) return;
	AD_DA_Slow_Down = 0;
	AD_1_Data_10x = Ad_Read(0x41) *10/51;
	AD_3_Data_10x = Ad_Read(0x43) *10/51;
	Da_Write(3*51);

}

void Distance_Get()
{
	if(Distance_Slow_Down < 150) return;
	Distance_Slow_Down = 0;
	Distance = Ut_Wave_Data();
}
void Timer1_Init(void)
{
	AUXR &= 0xBF;
	TMOD &= 0x0F;
	TL1 = 0x18;
	TH1 = 0xFC;
	TF1 = 0;
	TR1 = 1;
	ET1 = 1;
	EA = 1;
}

void Timer0_Init(void)		//1毫秒@12.000MHz
{
	AUXR &= 0x7F;			//定时器时钟12T模式
	TMOD &= 0xF0;			//设置定时器模式
	TMOD |= 0x05;			//设置定时器模式,计数器不重装载
	TL0 = 0x00;				//设置定时初始值
	TH0 = 0x00;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时

}


void main()
{
	System_Init();
	Set_Rtc(ucRtc);
	Timer0_Init();
	Timer1_Init();
	while(1)
	{
		Key_Proc();
		Led_Proc();
		Seg_Proc();
		Get_Time();
		Get_Temperature();
		AD_DA();
		Distance_Get();
	}
}
