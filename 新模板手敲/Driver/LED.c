#include "LED.H"

idata unsigned char temp_1 = 0x00;
idata unsigned char temp_1_old = 0xff;
//ucLed是且只能是0或1
void Led_Disp(unsigned char *ucLed)
{
	unsigned char temp;
	temp_1 = 0x00;
	temp_1 = (ucLed[0]<<0)|(ucLed[1]<<1)|(ucLed[2]<<2)|(ucLed[3]<<3)|
	(ucLed[4]<<4)|(ucLed[5]<<5)|(ucLed[6]<<6)|(ucLed[7]<<7);
	//当LED状态发生变化
	if(temp_1 != temp_1_old)
	{
		P0 = ~temp_1;
		
		temp = P2 & 0x1f;
		temp = temp|0x80;
		P2 = temp;
		temp = P2 & 0x1f;
		P2 = temp;
		
		temp_1_old = temp_1;
	}
}

void Led_Off()
{
	unsigned char temp;
	
	P0 = 0xff;
	
	temp = P2 & 0x1f;
	temp = temp|0x80;
	P2 = temp;
	temp = P2 & 0x1f;
	P2 = temp;
		
	temp_1_old = 0x00;//关掉后将old变为关闭
}

idata unsigned char temp_2 = 0x00;
idata unsigned char temp_2_old = 0xff;

void Beep(bit enable)
{
	unsigned char temp;
	if(enable)
	{
		temp_2 |= 0x40;
	}else
	{
		temp_2 &= (~0x40);
	}
	if(temp_2 != temp_2_old)
	{
		P0 = temp_2;
		
		temp = P2 & 0x1f;
		temp = temp|0x80;
		P2 = temp;
		temp = P2 & 0x1f;
		P2 = temp;
		
		temp_2_old = temp_2;
	}
}

void Motor(bit enable)
{
	unsigned char temp;
	if(enable)
	{
		temp_2 |= 0x20;
	}else
	{
		temp_2 &= (~0x20);
	}
	if(temp_2 != temp_2_old)
	{
		P0 = temp_2;
		
		temp = P2 & 0x1f;
		temp = temp|0x80;
		P2 = temp;
		temp = P2 & 0x1f;
		P2 = temp;
		
		temp_2_old = temp_2;
	}
}

void Relay(bit enable)
{
	unsigned char temp;
	if(enable)
	{
		temp_2 |= 0x10;
	}else
	{
		temp_2 &= (~0x10);
	}
	if(temp_2 != temp_2_old)
	{
		P0 = temp_2;
		
		temp = P2 & 0x1f;
		temp = temp|0x80;
		P2 = temp;
		temp = P2 & 0x1f;
		P2 = temp;
		
		temp_2_old = temp_2;
	}
}
