/**
 * @file    Seg.c
 * @brief   数码管显示驱动模块
 * @details CT107D开发板8位共阳数码管，通过74HC138+74HC573锁存器控制
 *          位选：Y6C锁存器(P2高3位=110=0xC0)
 *          段选：Y7C锁存器(P2高3位=111=0xE0)
 *          动态扫描方式：在定时器中断中逐位刷新，每次只点亮1位
 */
#include "Seg.h"

/**
 * @brief  共阳数码管段码表
 * @note   索引0~9对应数字0~9，索引10=0xFF(灭)
 *         共阳极：低电平点亮对应段
 */
pdata unsigned char Seg_Dula[] = {
	0xC0,  // 0
	0xF9,  // 1
	0xA4,  // 2
	0xB0,  // 3
	0x99,  // 4
	0x92,  // 5
	0x82,  // 6
	0xF8,  // 7
	0x80,  // 8
	0x90,  // 9
	0xFF,   // 10=灭(全段熄灭)
	0xC1,//11-U
	0x8c,//12-P
	0xAB,//13-n
	
	
};

/**
 * @brief   数码管单位显示函数(在定时器中断中调用)
 * @param   Wela   位选(0~7对应第1~8位数码管)
 * @param   Dula   段码索引(0~9为数字，10为灭)
 * @param   Point  小数点(1=显示小数点，0=不显示)
 * @note    调用顺序：消影 -> 位选 -> 段选，防止鬼影
 */
void Seg_Disp(unsigned char Wela, unsigned char Dula, bit Point)
{
	unsigned char temp;

	/* 消影：先将段选设为全灭，防止切换位选时产生鬼影 */
	P0 = 0xff;

	temp = P2 & 0x1f;
	temp = temp | 0xe0;     // 选通Y7C(段选锁存器)
	P2 = temp;
	temp = P2 & 0x1f;
	P2 = temp;

	/* 位选：选中目标数码管位 */
	P0 = 0x01 << Wela;     // 左移Wela位，选中对应数码管

	temp = P2 & 0x1f;
	temp = temp | 0xc0;     // 选通Y6C(位选锁存器)
	P2 = temp;
	temp = P2 & 0x1f;
	P2 = temp;

	/* 段选：输出段码 */
	P0 = Seg_Dula[Dula];   // 查表获取段码

	if(Point) P0 &= 0x7f;  // 显示小数点：将最高位(dp段)清零(共阳低电平点亮)

	temp = P2 & 0x1f;
	temp = temp | 0xe0;     // 选通Y7C(段选锁存器)
	P2 = temp;
	temp = P2 & 0x1f;
	P2 = temp;
}
