/**
 * @file    uart.c
 * @brief   串口1通信驱动模块
 * @details 配置：9600bps, 8位数据, 无校验, 1位停止位
 *          波特率发生器：定时器2(12T模式, 12MHz晶振)
 *          支持printf重定向输出
 */
#include "uart.h"
#include <stdio.h>

/**
 * @brief   串口1初始化(9600bps@12.000MHz)
 * @note    使用定时器2作为波特率发生器(12T模式)
 *          波特率计算：9600 = 12000000/12/(65536-T2初值)/32
 *          T2初值 = 65536 - 12000000/12/32/9600 ≈ 0xFFE6
 */
void Uart1_Init(void)
{
	SCON = 0x50;               // 模式1：8位UART，允许接收(REN=1)
	AUXR |= 0x01;              // 串口1选择定时器2为波特率发生器
	AUXR &= 0xFB;              // 定时器2时钟 = 系统时钟/12(12T模式)
	T2L = 0xE6;                // 定时器2初值低字节
	T2H = 0xFF;                // 定时器2初值高字节
	AUXR |= 0x10;              // 启动定时器2
	ES = 1;                    // 使能串口1中断
	EA = 1;                    // 使能总中断
}

/**
 * @brief   putchar重定向(支持printf通过串口输出)
 * @param   ch  要发送的字符
 * @return  发送的字符
 * @note    将字符写入SBUF，等待发送完成(TI=1)，然后清除TI标志
 */
extern char putchar(char ch)
{
	SBUF = ch;                 // 将字符写入发送缓冲区
	while(TI == 0)             // 等待发送完成
		;
	TI = 0;                    // 清除发送完成标志
	return ch;
}
