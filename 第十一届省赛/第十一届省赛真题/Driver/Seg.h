/**
 * @file    Seg.h
 * @brief   数码管显示驱动头文件
 */
#ifndef _SEG_H_
#define _SEG_H_
#include <STC15F2K60S2.H>

/**
 * @brief  数码管单位显示(定时器中断中调用)
 * @param  Wela   位选(0~7)
 * @param  Dula   段码索引(0~9数字, 10=灭)
 * @param  Point  小数点(1=显示, 0=不显示)
 */
void Seg_Disp(unsigned char Wela, unsigned char Dula, bit Point);

#endif
