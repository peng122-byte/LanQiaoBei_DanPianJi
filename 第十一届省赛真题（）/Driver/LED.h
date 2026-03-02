/**
 * @file    LED.h
 * @brief   LED与外设(蜂鸣器/电机/继电器)驱动头文件
 */
#ifndef _LED_H_
#define _LED_H_
#include <STC15F2K60S2.H>

void Led_Disp(unsigned char *ucLed);  /* LED显示，传入8字节数组(0=灭,1=亮) */
void Led_Off(void);                    /* 关闭所有LED(PWM灭灯周期用) */
void Beep(bit enable);                 /* 蜂鸣器控制(1=开,0=关) */
void Motor(bit enable);                /* 电机控制(1=开,0=关) */
void Relay(bit enable);                /* 继电器控制(1=开,0=关) */

#endif
