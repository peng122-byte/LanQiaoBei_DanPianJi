/**
 * @file    LED.c
 * @brief   LED与外设(蜂鸣器/电机/继电器)驱动模块
 * @details CT107D开发板外设控制：
 *          - LED:    Y4C锁存器(P2高3位=100=0x80)，P0低电平点亮
 *          - 蜂鸣器: Y5C锁存器(P2高3位=101=0xA0)，P0.6控制
 *          - 电机:   Y5C锁存器，P0.5控制
 *          - 继电器: Y5C锁存器，P0.4控制
 *          采用"脏标记"优化：仅在状态变化时操作锁存器，减少总线操作
 */
#include "LED.H"

/* ==================== LED控制 ==================== */

idata unsigned char temp_1 = 0x00;       /* LED当前状态(8位对应L1~L8) */
idata unsigned char temp_1_old = 0xff;   /* LED上次状态，初始化为不同值以触发首次刷新 */

/**
 * @brief   LED显示函数
 * @param   ucLed  8字节数组指针，每个元素0=灭，1=亮，对应L1~L8
 * @note    仅当LED状态发生变化时才操作锁存器，提高效率
 */
void Led_Disp(unsigned char *ucLed)
{
	unsigned char temp;

	/* 将8个独立的0/1值合并为一个字节 */
	temp_1 = 0x00;
	temp_1 = (ucLed[0]<<0)|(ucLed[1]<<1)|(ucLed[2]<<2)|(ucLed[3]<<3)|
	(ucLed[4]<<4)|(ucLed[5]<<5)|(ucLed[6]<<6)|(ucLed[7]<<7);

	/* 仅当LED状态发生变化时才刷新 */
	if(temp_1 != temp_1_old)
	{
		P0 = ~temp_1;              // 取反：硬件低电平点亮LED

		temp = P2 & 0x1f;         // 保留P2低5位
		temp = temp | 0x80;       // 选通Y4C(LED锁存器)
		P2 = temp;
		temp = P2 & 0x1f;         // 关闭选通
		P2 = temp;

		temp_1_old = temp_1;      // 更新旧状态
	}
}

/**
 * @brief   关闭所有LED
 * @note    用于PWM调光时的灭灯周期，关闭后将old标记重置以便下次能正确点亮
 */
void Led_Off()
{
	unsigned char temp;

	P0 = 0xff;                    // 全部熄灭(高电平=灭)

	temp = P2 & 0x1f;
	temp = temp | 0x80;           // 选通Y4C(LED锁存器)
	P2 = temp;
	temp = P2 & 0x1f;
	P2 = temp;

	temp_1_old = 0x00;            // 重置为"关闭"状态，确保下次Led_Disp能触发刷新
}

/* ==================== 蜂鸣器/电机/继电器控制 ==================== */

idata unsigned char temp_2 = 0x00;       /* Y5C锁存器当前状态 */
idata unsigned char temp_2_old = 0xff;   /* Y5C锁存器上次状态 */

/**
 * @brief   蜂鸣器控制
 * @param   enable  1=开启蜂鸣器, 0=关闭蜂鸣器
 * @note    蜂鸣器对应P0.6(0x40)
 */
void Beep(bit enable)
{
	unsigned char temp;
	if(enable)
	{
		temp_2 |= 0x40;           // 置位P0.6
	}else
	{
		temp_2 &= (~0x40);        // 清除P0.6
	}
	if(temp_2 != temp_2_old)      // 状态变化时才操作锁存器
	{
		P0 = temp_2;

		temp = P2 & 0x1f;
		temp = temp | 0xa0;       // 选通Y5C(蜂鸣器/继电器/电机锁存器)
		P2 = temp;
		temp = P2 & 0x1f;
		P2 = temp;

		temp_2_old = temp_2;
	}
}

/**
 * @brief   电机控制
 * @param   enable  1=开启电机, 0=关闭电机
 * @note    电机对应P0.5(0x20)
 */
void Motor(bit enable)
{
	unsigned char temp;
	if(enable)
	{
		temp_2 |= 0x20;           // 置位P0.5
	}else
	{
		temp_2 &= (~0x20);        // 清除P0.5
	}
	if(temp_2 != temp_2_old)
	{
		P0 = temp_2;

		temp = P2 & 0x1f;
		temp = temp | 0xa0;       // 选通Y5C
		P2 = temp;
		temp = P2 & 0x1f;
		P2 = temp;

		temp_2_old = temp_2;
	}
}

/**
 * @brief   继电器控制
 * @param   enable  1=开启继电器, 0=关闭继电器
 * @note    继电器对应P0.4(0x10)
 */
void Relay(bit enable)
{
	unsigned char temp;
	if(enable)
	{
		temp_2 |= 0x10;           // 置位P0.4
	}else
	{
		temp_2 &= (~0x10);        // 清除P0.4
	}
	if(temp_2 != temp_2_old)
	{
		P0 = temp_2;

		temp = P2 & 0x1f;
		temp = temp | 0xa0;       // 选通Y5C
		P2 = temp;
		temp = P2 & 0x1f;
		P2 = temp;

		temp_2_old = temp_2;
	}
}
