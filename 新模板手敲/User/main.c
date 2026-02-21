#include <STC15F2K60S2.H>
#include "LED.h"
#include "init.h"
#include "Seg.h"
#include "Key.h"
#include "ds1302.h"
#include "onewire.h"
#include "iic.h"
#include "ultrasound.h"
#include "uart.h"
#include "string.h"
#include "stdio.h"

idata unsigned long int uwTick;
pdata unsigned char ucLed[8] = {1,0,1,0,1,0,1,0};
pdata unsigned char Seg_Buf[8] = {10,10,10,10,10,10,10,10};
idata unsigned char Seg_Pos = 0;

idata unsigned char Key_Val,Key_Old,Key_Up,Key_Down;

idata unsigned char ucRtc[3] = {12,23,45};

idata unsigned int Temperature_10x;

idata unsigned char AD_1_Data_10x, AD_3_Data_10x;

pdata unsigned char EEPROM_Data_W[8] = {1,2,3,4,5,6,7,8};
pdata unsigned char EEPROM_Data_R[8] = {0,0,0,0,0,0,0,0};

idata unsigned char Distance;

idata unsigned int Freq;
idata unsigned int Time_1s;

idata unsigned char pwm_period;//累加对比
idata unsigned char pwm_compare = 6;//比较值，也就是亮度

idata unsigned char Uart_Rx_Index;
pdata unsigned char Uart_Rx_Buf[10] = {0,0,0,0,0,0,0,0,0,0};
idata unsigned char Uart_Rx_Flag;
idata unsigned char Uart_Rx_Tick;//超时解析计时器


idata unsigned char Seg_Show_Mod;//0 时间 1 温度 2 AD 3 超声波 4 频率 5 PWM 
void Key_Proc()
{
	

	Key_Val = Key_Read();
	Key_Down = Key_Val & (Key_Val ^ Key_Old);
	Key_Up = ~ Key_Val & (Key_Val ^ Key_Old);
	Key_Old = Key_Val;
	if(Key_Down == 4)
		pwm_compare= (++pwm_compare)%10;
	if(Key_Down != 0)
		printf("Key_Down: %bu\n",Key_Down);
	if(Key_Down == 5)
	{
		Seg_Show_Mod = (++Seg_Show_Mod)%6;
	}
}


void Seg_Proc()
{
	

	switch(Seg_Show_Mod)
	{
		case 0://0 时间 1 温度 2 AD 3 超声波 4 频率 5 PWM 
		Seg_Buf[0] = ucRtc[0]/10;
		Seg_Buf[1] = ucRtc[0]%10;
		Seg_Buf[2] = 10+',';
		Seg_Buf[3] = ucRtc[1]/10;
		Seg_Buf[4] = ucRtc[1]%10;
		Seg_Buf[5] = 10+',';
		Seg_Buf[6] = ucRtc[2]/10;
		Seg_Buf[7] = ucRtc[2]%10;
		
		break;
		case 1:
		Seg_Buf[0] = Temperature_10x/100;
		Seg_Buf[1] = Temperature_10x/10%10+',';
		Seg_Buf[2] = Temperature_10x%10;
		Seg_Buf[3] = 10;
		Seg_Buf[4] = 10;
		Seg_Buf[5] = 10;
		Seg_Buf[6] = 10;
		Seg_Buf[7] = 10;
			
		break;
		case 2:
		Seg_Buf[0] = AD_1_Data_10x/10+',';
		Seg_Buf[1] = AD_1_Data_10x%10;
		Seg_Buf[2] = 10;
		Seg_Buf[3] = 10;
		Seg_Buf[4] = 10;
		Seg_Buf[5] = 10;
		Seg_Buf[6] = AD_3_Data_10x/10+',';
		Seg_Buf[7] = AD_3_Data_10x%10;
		break;
		case 3:
		Seg_Buf[0] = Distance/100;
		Seg_Buf[1] = Distance/10%10;
		Seg_Buf[2] = Distance%10;
		Seg_Buf[3] = 10;
		Seg_Buf[4] = 10;
		Seg_Buf[5] = 10;
		Seg_Buf[6] = 10;
		Seg_Buf[7] = 10;
		break;
		case 4:
		Seg_Buf[0] = (Freq > 10000000)?Freq/10000000%10:10;
		Seg_Buf[1] = (Freq > 1000000)?Freq/1000000%10:10;
		Seg_Buf[2] = (Freq > 100000)?Freq/100000%10:10;
		Seg_Buf[3] = (Freq > 10000)?Freq/10000%10:10;
		Seg_Buf[4] = (Freq > 1000)?Freq/1000%10:10;
		Seg_Buf[5] = (Freq > 100)?Freq/100%10:10;
		Seg_Buf[6] = (Freq > 10)?Freq/10%10:10;
		Seg_Buf[7] = (Freq > 1)?Freq%10:10;
		break;
		case 5:
		Seg_Buf[0] = pwm_compare;
		Seg_Buf[1] = 10;
		Seg_Buf[2] = 10;
		Seg_Buf[3] = 10;
		Seg_Buf[4] = 10;
		Seg_Buf[5] = 10;
		Seg_Buf[6] = 10;
		Seg_Buf[7] = 10;
		break;
	}
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
	Read_Rtc(ucRtc);
}


void Get_Temperature()
{
	
	Temperature_10x = rd_temperature() *10;
}


void AD_DA()
{
	
	AD_1_Data_10x = Ad_Read(0x41) *10/51;
	AD_3_Data_10x = Ad_Read(0x43) *10/51;
	Da_Write(3*51);

}


void Distance_Get()
{
	
	Distance = Ut_Wave_Data();
}


void Uart_Proc()
{
	unsigned char x,y;
	if(Uart_Rx_Index == 0)return;
	if(Uart_Rx_Tick >= 10)
	{
		Uart_Rx_Flag = 0;
		Uart_Rx_Tick = 0;
		
		printf("%s",Uart_Rx_Buf);//回显，确保预期接收正常
		if(sscanf(Uart_Rx_Buf,"(%bu,%bu)",&x,&y) == 2)
			printf("\r\nI Get x = %bu, y = %bu\r\n",x,y);
		else
			printf("\r\nERROR\r\n");
		
		memset(Uart_Rx_Buf,0,Uart_Rx_Index);
		Uart_Rx_Index = 0;
	}
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


void Timer1_Isr(void) interrupt 3
{
	uwTick++;
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
	if(Uart_Rx_Flag) Uart_Rx_Tick ++;
}


void Uart1_Isr(void) interrupt 4
{
	if (RI)				//检测串口1接收中断
	{
		Uart_Rx_Flag = 1;
		Uart_Rx_Tick = 0;//重置计时器
		Uart_Rx_Buf[Uart_Rx_Index++] = SBUF;
		RI = 0;			//清除串口1接收中断请求位
		if(Uart_Rx_Index > 10)
		{
			Uart_Rx_Index = 0;
			memset(Uart_Rx_Buf,0,10);
		}
	}
}

typedef struct
{
	void(*task_func)(void);
	unsigned long int rate_ms;
	unsigned long int last_ms;
}task_t;
idata task_t Scheduler_Task[]={
	{Led_Proc,1,0},
	{Key_Proc,10,0},
	{Seg_Proc,20,0},
	{Get_Time,100,0},
	{Get_Temperature,300,0},
	{AD_DA,150,0},
	{Distance_Get,120,0},
	{Uart_Proc,10,0}
};
idata unsigned char task_num;
void Scheduler_Init()
{
	task_num = sizeof(Scheduler_Task)/sizeof(task_t);
}
void Scheduler_Run()
{
	unsigned char i;
	for(i=0;i<task_num;i++)
	{
		unsigned long int now_time = uwTick;
		if(now_time >= (Scheduler_Task[i].rate_ms+Scheduler_Task[i].last_ms))
		{
			Scheduler_Task[i].last_ms = now_time;
			Scheduler_Task[i].task_func();
		}
	}
}
void main()
{
	System_Init();
	Scheduler_Init();
	Set_Rtc(ucRtc);
	
	EEPROM_Read(EEPROM_Data_R,0,8);
	EEPROM_Write(EEPROM_Data_W,0,8);
	EEPROM_Read(EEPROM_Data_R,0,8);
	Timer0_Init();
	Uart1_Init();
	Timer1_Init();
	while(1)
	{
		Scheduler_Run();
	}
}
