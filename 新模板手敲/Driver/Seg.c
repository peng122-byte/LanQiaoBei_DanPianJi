#include "Seg.h"
//0 1 2 3 4 5 6 7 8 9 灭


pdata unsigned char Seg_Dula[] = {0xc0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xff};

void Seg_Disp(unsigned char Wela, unsigned char Dula, bit Point)
{
	unsigned char temp;
	
	//消影
	P0 = 0xff;
	
	temp = P2 & 0x1f;
	temp = temp | 0xe0;
	P2 = temp;
	temp = P2 & 0x1f;
	P2 = temp;
	
	//位选
	P0 = 0x01 << Wela;
	
	temp = P2 & 0x1f;
	temp = temp | 0xc0;
	P2 = temp;
	temp = P2 & 0x1f;
	P2 = temp;
	
	//段选
	P0 = Seg_Dula[Dula];
	
	if(Point) P0 &= 0x7f;
	
	temp = P2 & 0x1f;
	temp = temp | 0xe0;
	P2 = temp;
	temp = P2 & 0x1f;
	P2 = temp;
	
}









