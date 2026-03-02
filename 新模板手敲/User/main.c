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
#include "ds1302.h"
#include "onewire.h"
#include "iic.h"
#include "ultrasound.h"
#include "uart.h"
#include "string.h"
#include "stdio.h"

/* ==================== 全局变量定义 ==================== */

idata unsigned long int uwTick;                          /* 系统节拍计数器(1ms递增，由Timer1中断驱动) */

pdata unsigned char ucLed[8] = {1,0,1,0,1,0,1,0};       /* LED状态数组(0=灭,1=亮)，对应L1~L8 */
pdata unsigned char Seg_Buf[8] = {10,10,10,10,10,10,10,10}; /* 数码管显示缓冲区(10=灭,0~9=数字,加','=带小数点) */
idata unsigned char Seg_Pos = 0;                         /* 当前扫描的数码管位(0~7循环) */

idata unsigned char Key_Val, Key_Old, Key_Up, Key_Down;  /* 按键状态：当前值/上次值/松开事件/按下事件 */

idata unsigned char ucRtc[3] = {12,23,45};               /* RTC时间数组 [时,分,秒]，初始值12:23:45 */

idata unsigned int Temperature_10x;                       /* 温度值(放大10倍，如256=25.6°C) */

idata unsigned char AD_1_Data_10x, AD_3_Data_10x;        /* AD值(放大10倍，通道1光敏/通道3变阻器) */

pdata unsigned char EEPROM_Data_W[8] = {1,2,3,4,5,6,7,8}; /* EEPROM写入测试数据 */
pdata unsigned char EEPROM_Data_R[8] = {0,0,0,0,0,0,0,0}; /* EEPROM读取缓冲区 */

idata unsigned char Distance;                             /* 超声波测距值(单位cm) */

idata unsigned int Freq;                                  /* 频率测量值(Hz) */
idata unsigned int Time_1s;                               /* 1秒计时计数器(0~999，每1ms加1) */

idata unsigned char pwm_period;                           /* PWM周期累加计数(0~9循环) */
idata unsigned char pwm_compare = 6;                      /* PWM比较值(占空比=compare/10，6=60%) */

idata unsigned char Uart_Rx_Index;                        /* 串口接收缓冲区当前索引 */
pdata unsigned char Uart_Rx_Buf[10] = {0,0,0,0,0,0,0,0,0,0}; /* 串口接收缓冲区(最大10字节) */
idata unsigned char Uart_Rx_Flag;                         /* 串口接收标志(1=正在接收中) */
idata unsigned char Uart_Rx_Tick;                         /* 串口接收超时计数(ms)，超过10ms视为一帧结束 */

idata unsigned char Seg_Show_Mod;                         /* 数码管显示模式(0~5) */

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

	if(Key_Down == 4)                               // S4按下：调节PWM占空比
		pwm_compare = (++pwm_compare) % 10;         // 0~9循环

	if(Key_Down != 0)                               // 任意按键按下：串口打印键值(调试用)
		printf("Key_Down: %bu\n", Key_Down);

	if(Key_Down == 5)                               // S5按下：切换显示模式
	{
		Seg_Show_Mod = (++Seg_Show_Mod) % 6;        // 0~5循环
	}
}

/**
 * @brief   数码管显示任务(20ms周期)
 * @note    根据Seg_Show_Mod切换显示内容，更新Seg_Buf缓冲区：
 *          模式0: 时钟 (时-分-秒)
 *          模式1: 温度 (XX.X)
 *          模式2: AD值 (通道1.X --- 通道3.X)
 *          模式3: 超声波距离 (XXX cm)
 *          模式4: 频率计数 (最大8位)
 *          模式5: PWM占空比 (0~9)
 *
 *          小数点编码规则：段码索引 + ',' 表示该位带小数点
 *          在Timer1中断中判断：Seg_Buf[i] > 20 则减去','显示带小数点的数字
 */
void Seg_Proc()
{
	switch(Seg_Show_Mod)
	{
		case 0:  /* 时钟显示：HH-MM-SS */
		Seg_Buf[0] = ucRtc[0] / 10;       // 时-十位
		Seg_Buf[1] = ucRtc[0] % 10;       // 时-个位
		Seg_Buf[2] = 10 + ',';            // 分隔符'-'(用小数点替代)
		Seg_Buf[3] = ucRtc[1] / 10;       // 分-十位
		Seg_Buf[4] = ucRtc[1] % 10;       // 分-个位
		Seg_Buf[5] = 10 + ',';            // 分隔符'-'
		Seg_Buf[6] = ucRtc[2] / 10;       // 秒-十位
		Seg_Buf[7] = ucRtc[2] % 10;       // 秒-个位
		break;

		case 1:  /* 温度显示：XX.X */
		Seg_Buf[0] = Temperature_10x / 100;           // 十位
		Seg_Buf[1] = Temperature_10x / 10 % 10 + ','; // 个位+小数点
		Seg_Buf[2] = Temperature_10x % 10;             // 小数位
		Seg_Buf[3] = 10;  // 灭
		Seg_Buf[4] = 10;
		Seg_Buf[5] = 10;
		Seg_Buf[6] = 10;
		Seg_Buf[7] = 10;
		break;

		case 2:  /* AD值显示：通道1(X.X) --- 通道3(X.X) */
		Seg_Buf[0] = AD_1_Data_10x / 10 + ',';  // 通道1整数+小数点
		Seg_Buf[1] = AD_1_Data_10x % 10;         // 通道1小数
		Seg_Buf[2] = 10;
		Seg_Buf[3] = 10;
		Seg_Buf[4] = 10;
		Seg_Buf[5] = 10;
		Seg_Buf[6] = AD_3_Data_10x / 10 + ',';  // 通道3整数+小数点
		Seg_Buf[7] = AD_3_Data_10x % 10;         // 通道3小数
		break;

		case 3:  /* 超声波距离显示：XXX cm */
		Seg_Buf[0] = Distance / 100;       // 百位
		Seg_Buf[1] = Distance / 10 % 10;   // 十位
		Seg_Buf[2] = Distance % 10;        // 个位
		Seg_Buf[3] = 10;
		Seg_Buf[4] = 10;
		Seg_Buf[5] = 10;
		Seg_Buf[6] = 10;
		Seg_Buf[7] = 10;
		break;

		case 4:  /* 频率显示：最多8位，高位自动消零 */
		Seg_Buf[0] = (Freq > 10000000) ? Freq / 10000000 % 10 : 10;
		Seg_Buf[1] = (Freq > 1000000)  ? Freq / 1000000  % 10 : 10;
		Seg_Buf[2] = (Freq > 100000)   ? Freq / 100000   % 10 : 10;
		Seg_Buf[3] = (Freq > 10000)    ? Freq / 10000    % 10 : 10;
		Seg_Buf[4] = (Freq > 1000)     ? Freq / 1000     % 10 : 10;
		Seg_Buf[5] = (Freq > 100)      ? Freq / 100      % 10 : 10;
		Seg_Buf[6] = (Freq > 10)       ? Freq / 10       % 10 : 10;
		Seg_Buf[7] = (Freq > 1)        ? Freq            % 10 : 10;
		break;

		case 5:  /* PWM占空比显示：0~9 */
		Seg_Buf[0] = pwm_compare;          // 直接显示占空比档位
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

/**
 * @brief   LED显示任务(1ms周期)
 * @note    设置LED状态数组，实际点亮由Timer1中断中的PWM控制
 */
void Led_Proc()
{
	ucLed[0] = 1;  // L1亮
	ucLed[1] = 0;  // L2灭
	ucLed[2] = 1;  // L3亮
	ucLed[3] = 0;  // L4灭
	ucLed[4] = 1;  // L5亮
	ucLed[5] = 0;  // L6灭
	ucLed[6] = 1;  // L7亮
	ucLed[7] = 0;  // L8灭
	//Led_Disp(ucLed);  // 此处不直接调用，由中断PWM控制亮度
}

/**
 * @brief   读取RTC时间任务(100ms周期)
 */
void Get_Time()
{
	Read_Rtc(ucRtc);
}

/**
 * @brief   读取温度任务(300ms周期)
 * @note    温度值放大10倍存储，保留1位小数(如25.6°C -> 256)
 */
void Get_Temperature()
{
	Temperature_10x = rd_temperature() * 10;
}

/**
 * @brief   AD采集与DA输出任务(150ms周期)
 * @note    AD值转换为电压放大10倍：raw * 10 / 51 ≈ raw * 5 / 255 * 10
 *          DA输出3V：3 * 51 = 153 ≈ 3V对应的数字量
 */
void AD_DA()
{
	AD_1_Data_10x = Ad_Read(0x41) * 10 / 51;  // 通道1(光敏)，电压*10
	AD_3_Data_10x = Ad_Read(0x43) * 10 / 51;  // 通道3(变阻器)，电压*10
	Da_Write(3 * 51);                           // DA输出约3V
}

/**
 * @brief   超声波测距任务(120ms周期)
 */
void Distance_Get()
{
	Distance = Ut_Wave_Data();
}

/**
 * @brief   串口数据处理任务(10ms周期)
 * @note    接收策略：基于超时判断帧结束(10ms无新数据视为一帧完成)
 *          解析格式：(x,y)，使用sscanf提取两个数值
 *          处理完毕后清空缓冲区
 */
void Uart_Proc()
{
	unsigned char x, y;

	if(Uart_Rx_Index == 0) return;            // 无数据，直接返回

	if(Uart_Rx_Tick >= 10)                     // 超时10ms，认为一帧接收完成
	{
		Uart_Rx_Flag = 0;
		Uart_Rx_Tick = 0;

		printf("%s", Uart_Rx_Buf);             // 回显接收到的原始数据(调试用)

		/* 尝试解析 (x,y) 格式的数据 */
		if(sscanf(Uart_Rx_Buf, "(%bu,%bu)", &x, &y) == 2)
			printf("\r\nI Get x = %bu, y = %bu\r\n", x, y);
		else
			printf("\r\nERROR\r\n");

		/* 清空接收缓冲区 */
		memset(Uart_Rx_Buf, 0, Uart_Rx_Index);
		Uart_Rx_Index = 0;
	}
}

/* ==================== 定时器初始化 ==================== */

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

/**
 * @brief   定时器0初始化(计数器模式，用于频率测量)
 * @note    12T模式，16位计数器(TMOD |= 0x05)
 *          T0引脚(P3.4)接收外部脉冲，硬件自动计数
 *          每1秒由Timer1中断读取计数值并清零 -> 得到频率(Hz)
 */
void Timer0_Init(void)
{
	AUXR &= 0x7F;             // 定时器0时钟 = 系统时钟/12(12T模式)
	TMOD &= 0xF0;             // 清除定时器0模式位
	TMOD |= 0x05;             // 模式5：16位计数器(不自动重装)，外部脉冲计数
	TL0 = 0x00;               // 计数器清零
	TH0 = 0x00;
	TF0 = 0;                  // 清除溢出标志
	TR0 = 1;                  // 启动定时器0(开始计数)
}

/* ==================== 中断服务函数 ==================== */

/**
 * @brief   定时器1中断服务函数(1ms周期)，中断号3
 * @note    承担5项职责：
 *          1. 系统节拍uwTick递增(驱动任务调度器)
 *          2. 数码管动态扫描(8位轮流显示)
 *          3. 频率计数(每1000次=1秒读取Timer0计数值)
 *          4. LED的PWM调光(10级占空比)
 *          5. 串口接收超时计数
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

	/* 频率测量：每1秒(1000ms)读取Timer0计数值 */
	if(++Time_1s == 1000)
	{
		Time_1s = 0;
		Freq = TH0 << 8 | TL0;   // 读取Timer0计数值 = 1秒内的脉冲数 = 频率(Hz)
		TH0 = TL0 = 0;           // 清零计数器，开始下一秒计数
	}

	/* LED的PWM调光(10级，周期10ms) */
	pwm_period = (++pwm_period) % 10;
	if(pwm_period < pwm_compare)
		Led_Disp(ucLed);           // PWM高电平期间：点亮LED
	else
		Led_Off();                 // PWM低电平期间：熄灭LED(实现调光效果)

	/* 串口接收超时计数 */
	if(Uart_Rx_Flag) Uart_Rx_Tick++;  // 正在接收时，每1ms计数+1
}

/**
 * @brief   串口1接收中断服务函数，中断号4
 * @note    每接收到一个字节触发一次：
 *          1. 置接收标志，重置超时计数器
 *          2. 将数据存入缓冲区(最大10字节防溢出)
 *          3. 清除RI接收中断标志
 */
void Uart1_Isr(void) interrupt 4
{
	if (RI)
	{
		Uart_Rx_Flag = 1;                       // 标记正在接收
		Uart_Rx_Tick = 0;                       // 重置超时计数(每收到新字节就清零)
		if(Uart_Rx_Index < 10)                  // 防止缓冲区溢出
			Uart_Rx_Buf[Uart_Rx_Index++] = SBUF;  // 存入缓冲区
		RI = 0;                                 // 清除接收中断标志
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
 *          低频任务：RTC(100ms)、超声波(120ms)、AD/DA(150ms)、温度(300ms)
 */
idata task_t Scheduler_Task[] = {
	{Led_Proc,        1,   0},       // LED状态更新(1ms)
	{Key_Proc,        10,  0},       // 按键扫描(10ms)
	{Seg_Proc,        20,  0},       // 数码管显示更新(20ms)
	{Get_Time,        100, 0},       // RTC时间读取(100ms)
	{Get_Temperature, 300, 0},       // 温度采集(300ms)
	{AD_DA,           150, 0},       // AD采集与DA输出(150ms)
	{Distance_Get,    120, 0},       // 超声波测距(120ms)
	{Uart_Proc,       10,  0}        // 串口数据处理(10ms)
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
 *          3. Set_Rtc()      - 设置RTC初始时间
 *          4. EEPROM读写测试  - 验证EEPROM功能
 *          5. Timer0_Init()  - 频率计数器
 *          6. Uart1_Init()   - 串口通信
 *          7. Timer1_Init()  - 系统节拍(最后初始化，启动中断)
 *          8. 主循环运行调度器
 */
void main()
{
	System_Init();                     // 系统初始化(关闭所有外设)
	Scheduler_Init();                  // 调度器初始化
	Set_Rtc(ucRtc);                    // 设置RTC初始时间(12:23:45)

	/* EEPROM读写测试 */
	EEPROM_Read(EEPROM_Data_R, 0, 8);  // 先读取当前EEPROM内容
	EEPROM_Write(EEPROM_Data_W, 0, 8); // 写入测试数据{1,2,3,4,5,6,7,8}
	EEPROM_Read(EEPROM_Data_R, 0, 8);  // 再次读取验证写入是否成功

	Timer0_Init();                      // 初始化频率计数器(Timer0计数模式)
	Uart1_Init();                       // 初始化串口(9600bps)
	Timer1_Init();                      // 初始化系统节拍(1ms中断，最后启动)

	while(1)
	{
		Scheduler_Run();                // 持续运行任务调度器
	}
}
