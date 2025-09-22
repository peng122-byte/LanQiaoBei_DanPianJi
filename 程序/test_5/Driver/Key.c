#include "Key.h"


unsigned char Key_Read()
{
	unsigned char temp=0;
	P3_0=0;P3_1=1;P3_2=1;P3_3=1;
	if(P3_4==0) temp=1;
	if(P3_4==0) temp=1;
	if(P3_4==0) temp=1;
	if(P3_4==0) temp=1;
	P3_0=1;P3_1=0;P3_2=1;P3_3=1;
	if(P3_4==0) temp=1;
	if(P3_4==0) temp=1;
	if(P3_4==0) temp=1;
	if(P3_4==0) temp=1;
	P3_0=1;P3_1=1;P3_2=0;P3_3=1;
	if(P3_4==0) temp=1;
	if(P3_4==0) temp=1;
	if(P3_4==0) temp=1;
	if(P3_4==0) temp=1;
	P3_0=1;P3_1=1;P3_2=1;P3_3=0;
	if(P3_4==0) temp=1;
	if(P3_4==0) temp=1;
	if(P3_4==0) temp=1;
	if(P3_4==0) temp=1;
	
}