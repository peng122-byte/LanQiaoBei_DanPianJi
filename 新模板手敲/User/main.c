#include <STC15F2K60S2.H>
#include "LED.h"


pdata unsigned char ucLed[8] = {1,0,1,0,1,0,1,0};





void main()
{
	Led_Disp(ucLed);
	Beep(1);
	Motor(1);
	Relay(1);
	
	
	
	
	
	while(1)
	{
		
	}
}