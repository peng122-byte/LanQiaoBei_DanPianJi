#include <STC15F2K60S2.H>
#include "LED.h"
#include "init.h"
#include "Seg.h"
#include "Key.h"
#include "ds1302.h"
#include "onewire.h"
#include "iic.h"
pdata unsigned char ucLed[8] = {1,0,1,0,1,0,1,0};
pdata unsigned char Seg_Buf[8] = {10,10,10,10,10,10,10,10};
idata unsigned char Seg_Pos = 0;
idata unsigned char Seg_Slow_Down;

idata unsigned char Key_Val,Key_Old,Key_Up,Key_Down;
idata unsigned char Key_Slow_Down;

//idata unsigned char Key_Down_Data , Key_Up_Data;//Key调试用

idata unsigned char ucRtc[3] = {11,12,13};
idata unsigned char Time_Slow_Down;

idata unsigned int Temperature_10x;
idata unsigned int Temperature_Slow_Down;

idata unsigned char AD_1_Data_10x, AD_3_Data_10x;
idata unsigned char AD_DA_Slow_Down;

pdata unsigned char EEPROM_Data_W[8] = {1,2,3,4,5,6,7,8};
pdata unsigned char EEPROM_Data_R[8] = {0,0,0,0,0,0,0,0};

idata unsigned char Led_PWM = 0;      // PWM计数器 (0-9循环)
idata unsigned char Led_Level = 5;    // PWM占空比等级 (0-10, 5=50%)


void Key_Proc()
{
	if(Key_Slow_Down < 10) return;
	Key_Slow_Down = 0;
	
	Key_Val = Key_Read();
	Key_Down = Key_Val & (Key_Val ^ Key_Old);
	Key_Up = ~ Key_Val & (Key_Val ^ Key_Old);
	Key_Old = Key_Val;
	/*测试按键*/
//	if(Key_Down != 0)
//		Key_Down_Data = Key_Down;
//	if(Key_Up != 0)
//		Key_Up_Data = Key_Up;
}


void Timer1_Isr(void) interrupt 3
{
	Seg_Slow_Down ++;
	Key_Slow_Down ++;
	Time_Slow_Down ++;
	Temperature_Slow_Down ++;
	AD_DA_Slow_Down ++;
	Seg_Pos = (++Seg_Pos) % 8;
	if(Seg_Buf[Seg_Pos] > 20) 
	{
		Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos]- ',',1);
	}else
	{
		Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos],0);
	}
	// ========== LED PWM亮度控制 ==========
  if(++Led_PWM >= 10) Led_PWM = 0;  // 计数器循环 0-9

  if(Led_PWM < Led_Level)
  {
    Led_Disp(ucLed);  // 占空比内：显示LED
  }
  else
 {
  // 占空比外：关闭所有LED
   P0 = 0xFF;  // LED全灭（根据硬件电路调整）
   P2 = (P2 & 0x1F) | 0x80;
   P2 &= 0x1F;
 }
}


void Seg_Proc()
{
	if(Seg_Slow_Down < 20) return;
	Seg_Slow_Down = 0;
	
}

void Led_Proc()
{
	if(Temperature_10x > 300)       // 温度 > 30°C
    Led_Level = 9;              // 高亮
  else if(Temperature_10x > 200)  // 温度 > 20°C
    Led_Level = 5;              // 中等亮度
  else
    Led_Level = 2;              // 低亮度
	Led_Disp(ucLed);
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

void Timer1_Init(void)		//1毫秒@12.000MHz
{
	AUXR &= 0xBF;			//定时器时钟12T模式
	TMOD &= 0x0F;			//设置定时器模式
	TL1 = 0x18;				//设置定时初始值
	TH1 = 0xFC;				//设置定时初始值
	TF1 = 0;				//清除TF1标志
	TR1 = 1;				//定时器1开始计时
	ET1 = 1;				//使能定时器1中断
	EA =1 ;
}





void main()
{
	System_Init();
	Set_Rtc(ucRtc);
	
	Timer1_Init();
	while(1)
	{
		Key_Proc();
		Led_Proc();
		Seg_Proc();
		Get_Time();
		Get_Temperature();
		AD_DA();
	}
}