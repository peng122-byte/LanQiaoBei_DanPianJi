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

