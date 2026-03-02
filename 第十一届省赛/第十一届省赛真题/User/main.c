/**
 * @file    main.c
 * @brief   蓝桥杯单片机竞赛综合模板 - 主程序
 * @details MCU: STC15F2K60S2, 晶振: 12MHz
 *          架构: 基于时间片的协作式任务调度器
 *          功能: 时钟/温度/AD-DA/超声波/频率/PWM/串口
 */
#include <STC15F2K60S2.H>
#include "LED.h"
#include "init.h"
#include "Seg.h"
#include "Key.h"
#include "iic.h"

/* ==================== 全局变量定义 ==================== */

idata unsigned long int uwTick;                          /* 系统节拍计数器(1ms递增，由Timer1中断驱动) */

pdata unsigned char ucLed[8] = {0,0,0,0,0,0,0,0};       /* LED状态数组(0=灭,1=亮)，对应L1~L8 */
pdata unsigned char Seg_Buf[8] = {10,10,10,10,10,10,10,10}; /* 数码管显示缓冲区(10=灭,0~9=数字,加','=带小数点) */
idata unsigned char Seg_Pos = 0;                         /* 当前扫描的数码管位(0~7循环) */
idata unsigned char Key_Val, Key_Old, Key_Up, Key_Down;  /* 按键状态：当前值/上次值/松开事件/按下事件 */
idata unsigned int AD_3_Data_100x;        /* AD值(放大10倍，通道1光敏/通道3变阻器) */
pdata unsigned char EEPROM_Data_W[8] = {1,2,3,4,5,6,7,8}; /* EEPROM写入测试数据 */
pdata unsigned char EEPROM_Data_R[8] = {0,0,0,0,0,0,0,0}; /* EEPROM读取缓冲区 */
idata unsigned char Seg_Show_Mod;                         /* 数码管显示模式(0~5) */
idata unsigned char Voltage_E2PROM;
idata unsigned int Voltage_Get, Voltage_Set = 300;/*电压设置参数*/
idata unsigned char Voltage_Count;/*电压计数*/
idata unsigned int Time5000;//5秒
bit Flag;//下降沿标志位
idata unsigned char Error;//错误处理 
/* ==================== 任务处理函数 ==================== */

/**
 * @brief   按键处理任务(10ms周期)
 * @note    按键检测采用"按下沿捕获"：
 *          Key_Down = Key_Val & (Key_Val ^ Key_Old) —— 检测新按下
 *          Key_Up   = ~Key_Val & (Key_Val ^ Key_Old) —— 检测释放
 *
 *          S4: 调节PWM占空比(0~9档循环)
 *          S5: 切换数码管显示模式(0~5循环)
 */
void Key_Proc()
{
	Key_Val = Key_Read();                          // 读取当前按键值
	Key_Down = Key_Val & (Key_Val ^ Key_Old);      // 检测按下沿(由无到有)
	Key_Up = ~Key_Val & (Key_Val ^ Key_Old);       // 检测释放沿(由有到无)
	Key_Old = Key_Val;                              // 保存当前值供下次比较
	switch(Key_Down)
	{
		case 12:
			if(++Seg_Show_Mod == 2) 
			{
				Voltage_E2PROM = Voltage_Set/10;
				EEPROM_Write(&Voltage_E2PROM, 0, 1);
			}
			if(Seg_Show_Mod == 3) Seg_Show_Mod = 0;
			Error = 0;
			
		break;
		case 13:
			if(Seg_Show_Mod == 2) 
			{
				Error = 0;
				Voltage_Count = 0;
			}
			else Error ++;
			
		break;
		case 16:
			if(Seg_Show_Mod == 1) 
			{
				Error = 0;
				Voltage_Set += 50;
				if(Voltage_Set > 500) Voltage_Set = 0;
			}
			else Error ++ ;
			
		break;
		case 17:
			if(Seg_Show_Mod == 1) 
			{
				Error = 0;
				if(Voltage_Set == 0) Voltage_Set = 500;
				Voltage_Set -= 50;
			}
			else Error ++ ;
		break;
	}
	
}


void Seg_Proc()
{
	switch(Seg_Show_Mod)
	{
		case 0:
			Seg_Buf[0] = 11;
			Seg_Buf[1] = 10;
			Seg_Buf[2] = 10;
			Seg_Buf[3] = 10;
			Seg_Buf[4] = 10;
			Seg_Buf[5] = AD_3_Data_100x /100 %10 +',';
			Seg_Buf[6] = AD_3_Data_100x /10 %10;
			Seg_Buf[7] = AD_3_Data_100x /1 %10;
		break;
		case 1:
			Seg_Buf[0] = 12;
			Seg_Buf[1] = 10;
			Seg_Buf[2] = 10;
			Seg_Buf[3] = 10;
			Seg_Buf[4] = 10;
			Seg_Buf[5] = Voltage_Set /100 %10 +',';
			Seg_Buf[6] = Voltage_Set /10 %10;
			Seg_Buf[7] = Voltage_Set /1 %10;
		break;
		case 2:
			Seg_Buf[0] = 13;
			Seg_Buf[1] = 10;
			Seg_Buf[2] = 10;
			Seg_Buf[3] = 10;
			Seg_Buf[4] = 10;
			Seg_Buf[5] = 10;
			Seg_Buf[6] = Voltage_Count /10 %10;
			Seg_Buf[7] = Voltage_Count /1 %10;
		break;
	}
}


void Led_Proc()
{
	Voltage_Get = AD_3_Data_100x;
	if(Voltage_Get >= Voltage_Set) 
	{
		Time5000 = 0;
		Flag = 1;
	}
	if((Flag) && (Voltage_Get < Voltage_Set))
	{
		Flag = 0;
		Voltage_Count ++;
	}
	if(Time5000 >= 5000) 
	{
		
		ucLed[0] = 1;
	}
	else ucLed[0] = 0;
	if(Voltage_Count%2) ucLed[1] = 1;
		else ucLed[1] = 0;
	if(Error >= 3) ucLed[2] = 1;
		else ucLed[2] = 0;
	
}

/**
 * @brief   AD采集与DA输出任务(150ms周期)
 * @note    AD值转换为电压放大100倍：raw * 100 / 51 ≈ raw * 5 / 255 * 100
 *          DA输出3V：3 * 51 = 153 ≈ 3V对应的数字量
 */
void AD_DA()
{
	AD_3_Data_100x = Ad_Read(0x43) * 100 / 51;  // 通道3(变阻器)，电压*10
}

/* =================== 定时器初始化 ==================== */

/**
 * @brief   定时器1初始化(1ms定时中断@12MHz)
 * @note    12T模式，16位自动重装
 *          初值：0xFC18 = 65536 - 1000 = 64536 (12MHz/12 = 1MHz, 1000us = 1ms)
 *          用途：系统节拍、数码管扫描、PWM、频率计数、串口超时
 */
void Timer1_Init(void)
{
	AUXR &= 0xBF;             // 定时器1时钟 = 系统时钟/12(12T模式)
	TMOD &= 0x0F;             // 清除定时器1模式位，使用模式0(16位自动重装)
	TL1 = 0x18;               // 定时初值低字节
	TH1 = 0xFC;               // 定时初值高字节
	TF1 = 0;                  // 清除溢出标志
	TR1 = 1;                  // 启动定时器1
	ET1 = 1;                  // 使能定时器1中断
	EA = 1;                   // 使能总中断
}


/* ==================== 中断服务函数 ==================== */

/**
 * @brief   定时器1中断服务函数(1ms周期)，中断号3
 * @note    承担5项职责：
 *          1. 系统节拍uwTick递增(驱动任务调度器)
 *          2. 数码管动态扫描(8位轮流显示)
 */
void Timer1_Isr(void) interrupt 3
{
	uwTick++;                  // 系统节拍+1

	/* 数码管动态扫描：每次中断显示下一位 */
	Seg_Pos = (++Seg_Pos) % 8;
	if(Seg_Buf[Seg_Pos] > 20)                              // >20说明带小数点标记(数字+',')
	{
		Seg_Disp(Seg_Pos, Seg_Buf[Seg_Pos] - ',', 1);      // 减去','恢复段码索引，第3参数1=显示小数点
	}else
	{
		Seg_Disp(Seg_Pos, Seg_Buf[Seg_Pos], 0);            // 正常显示，无小数点
	}
	Led_Disp(ucLed);
	if(Flag == 0)
	{
		Time5000++;
	}
	
	}




/* ==================== 任务调度器 ==================== */

/**
 * @brief   任务结构体定义
 * @details 每个任务包含：函数指针、执行周期(ms)、上次执行时间
 */
typedef struct
{
	void(*task_func)(void);          // 任务函数指针
	unsigned long int rate_ms;       // 执行周期(ms)
	unsigned long int last_ms;       // 上次执行时的uwTick值
} task_t;

/**
 * @brief   任务列表(静态配置)
 * @note    各任务按功能和频率需求分配周期：
 *          高频任务：LED(1ms)、按键/串口(10ms)、数码管(20ms)
 *          低频任务：AD/DA(150ms)
 */
idata task_t Scheduler_Task[] = {
	{Led_Proc,        1,   0},       // LED状态更新(1ms)
	{Key_Proc,        10,  0},       // 按键扫描(10ms)
	{Seg_Proc,        20,  0},       // 数码管显示更新(20ms)
	{AD_DA,           150, 0},       // AD采集与DA输出(150ms)
};

idata unsigned char task_num;        // 任务总数

/**
 * @brief   调度器初始化
 * @note    自动计算任务数量，无需手动维护
 */
void Scheduler_Init()
{
	task_num = sizeof(Scheduler_Task) / sizeof(task_t);
}

/**
 * @brief   调度器运行(在主循环中持续调用)
 * @note    遍历所有任务，检查是否到达执行周期：
 *          当前时间 - 上次执行时间 >= 任务周期 -> 执行任务
 *          非抢占式：任务执行期间不会被其他任务打断(中断除外)
 */
void Scheduler_Run()
{
	unsigned char i;
	for(i=0; i<task_num; i++)
	{
		unsigned long int now_time = uwTick;
		if(now_time >= (Scheduler_Task[i].rate_ms + Scheduler_Task[i].last_ms))
		{
			Scheduler_Task[i].last_ms = now_time;  // 记录本次执行时间
			Scheduler_Task[i].task_func();          // 调用任务函数
		}
	}
}

/* ==================== 主函数 ==================== */

/**
 * @brief   程序入口
 * @note    初始化顺序：
 *          1. System_Init()  - 关闭所有外设，防止误动作
 *          2. Scheduler_Init() - 初始化任务调度器
 *          4. EEPROM读写测试  - 验证EEPROM功能
 *          7. Timer1_Init()  - 系统节拍(最后初始化，启动中断)
 *          8. 主循环运行调度器
 */
void main()
{
	System_Init();                     // 系统初始化(关闭所有外设)
	Scheduler_Init();                  // 调度器初始化
	Timer1_Init();                      // 初始化系统节拍(1ms中断，最后启动)
	EEPROM_Read(&Voltage_E2PROM,0,1);
	Voltage_Set = Voltage_E2PROM *10;
	Ad_Read(0x43);
	while(1)
	{
		Scheduler_Run();                // 持续运行任务调度器
	}
}
