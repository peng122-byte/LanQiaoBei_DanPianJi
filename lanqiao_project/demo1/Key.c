#include <REGX52.H>
#include "Key.h"
unsigned char Key_read()
{
	unsigned char temp=0;
	if(P34==0)temp=1;
	if(P35==0)temp=2;
	if(P36==0)temp=3;
	if(P37==0)temp=4;
	return temp;
	
}