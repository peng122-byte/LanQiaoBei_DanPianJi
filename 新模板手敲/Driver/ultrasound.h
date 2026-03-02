/**
 * @file    ultrasound.h
 * @brief   超声波测距模块驱动头文件
 */
#ifndef _ULTRASOUND_H_
#define _ULTRASOUND_H_
#include <STC15F2K60S2.H>

unsigned char Ut_Wave_Data(void);  /* 超声波测距，返回距离(cm)，超时返回0 */

#endif
