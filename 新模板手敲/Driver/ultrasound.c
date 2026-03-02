/**
 * @file    ultrasound.c
 * @brief   超声波测距模块驱动
 * @details 原理：发送40kHz超声波脉冲(8个周期)，等待回波信号
 *          通过PCA计时器测量发射到接收的时间差，计算距离
 *          距离 = 时间 * 声速(340m/s) / 2 = time * 0.017 (单位cm)
 *          PCA计时器使用系统时钟/12模式
 */
#include "ultrasound.h"
#include "intrins.h"

sbit US_TX = P1^0;    // 超声波发射引脚
sbit US_RX = P1^1;    // 超声波接收引脚

/**
 * @brief   12us延时函数(用于产生40kHz方波)
 * @note    40kHz方波周期=25us，半周期约12.5us
 *          需根据实际晶振频率微调循环次数
 */
void Delay12us(void)  //@12.000MHz
{
	unsigned char data i;

	_nop_();
	i = 3;
	while (--i);
}

/**
 * @brief   发送超声波脉冲
 * @note    产生8个40kHz方波周期的超声波脉冲
 *          发送期间关闭中断EA，防止时序被打断
 */
void Ut_Wave_Init()
{
	unsigned char i;
	EA = 0;                    // 关中断，保证脉冲时序精确
	for(i=0; i<8; i++)         // 发送8个周期的40kHz方波
	{
		US_TX = 1;
		Delay12us();           // 高电平保持约12us
		US_TX = 0;
		Delay12us();           // 低电平保持约12us
	}
	EA = 1;                    // 恢复中断
}

/**
 * @brief   超声波测距(发送+接收+计算)
 * @return  测量距离(单位cm)，超时返回0
 * @note    使用STC15的PCA模块作为高精度计时器：
 *          CMOD=0x00: 系统时钟/12作为PCA时钟源
 *          CR=1启动计时，等待US_RX接收到回波(RX从高变低)
 *          CF=1表示PCA溢出(超时，约5.5ms，对应距离>90cm)
 *          距离 = time * 0.017cm (声速340m/s，来回除以2)
 */
unsigned char Ut_Wave_Data()
{
	unsigned int time;

	CMOD = 0x00;               // PCA时钟源 = 系统时钟/12
	CH = CL = 0;               // 清零PCA计数器
	Ut_Wave_Init();            // 发送超声波脉冲
	CR = 1;                    // 启动PCA计时

	/* 等待回波或超时 */
	while((US_RX == 1) && (CF == 0))  // RX=1未收到回波，CF=0未溢出
		;

	CR = 0;                    // 停止PCA计时

	if(CF == 0)                // 正常收到回波
	{
		time = CH << 8 | CL;   // 获取PCA计数值
		return (time * 0.017); // 计算距离(cm)
	}
	else                       // PCA溢出，超时
	{
		CF = 0;                // 清除溢出标志
		return 0;              // 返回0表示超出测量范围
	}
}
