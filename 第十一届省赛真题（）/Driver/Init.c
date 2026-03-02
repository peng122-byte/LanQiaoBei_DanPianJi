/**
 * @file    Init.c
 * @brief   系统初始化模块
 * @details 关闭所有外设输出(LED、蜂鸣器、继电器、电机)，防止上电后外设误动作
 *          CT107D开发板通过74HC138+74HC573锁存器控制外设，
 *          P2高3位选择锁存器，P0为数据总线
 */
#include "init.h"

/**
 * @brief  系统上电初始化
 * @note   必须在main函数最开始调用，确保所有外设处于关闭状态
 *         锁存器选通原理：P2高3位设置目标值 -> 锁存 -> P2高3位清零(关闭选通)
 */
void System_Init()
{
	unsigned char temp;

	/* 关闭LED: Y4C(P2高3位=100=0x80)锁存器，P0=0xFF全灭(低电平点亮) */
	P0 = 0xff;

	temp = P2 & 0x1f;       // 保留P2低5位不变
	temp = temp | 0x80;     // P2高3位=100，选通Y4C(LED锁存器)
	P2 = temp;
	temp = P2 & 0x1f;       // 清除P2高3位，关闭选通
	P2 = temp;

	/* 关闭蜂鸣器、继电器、电机: Y5C(P2高3位=101=0xA0)锁存器，P0=0x00全关 */
	P0 = 0x00;
	temp = P2 & 0x1f;
	temp = temp | 0xa0;     // P2高3位=101，选通Y5C(蜂鸣器/继电器/电机锁存器)
	P2 = temp;
	temp = P2 & 0x1f;
	P2 = temp;
}
