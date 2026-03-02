/**
 * @file    Key.h
 * @brief   矩阵键盘驱动头文件
 */
#ifndef _KEY_H_
#define _KEY_H_
#include <STC15F2K60S2.H>

unsigned char Key_Read(void);  /* 读取按键值，返回键号(4~19)或0(无按键) */

#endif
