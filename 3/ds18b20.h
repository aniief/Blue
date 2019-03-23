//2019-2-24;8:18开始******//
#include "reg52.h"
sbit DQ = P1^4;
void delay(unsigned int t)//t*1.35us
{
	while(t--);
}
bit Init_DS18B20()
{
	bit InitFlag;
	EA = 0;
	DQ = 1;
	delay(1);
	DQ = 0;
	delay(365);//480
	DQ = 1;
	delay(45);//60
	InitFlag = DQ;
	EA = 1;
	delay(180);//240
	return InitFlag;
}
void Write_DS18B20(unsigned char dat)
{
	unsigned char i;
	DQ = 1;
	for(i=0;i<8;i++)
	{
		EA = 0;
		DQ = 0;
		delay(1);//2
		DQ = dat&0x01;
		dat >>= 1;
		delay(50);
		DQ = 1;
		delay(1);
		EA = 1;
	}
}

unsigned char Read_DS18B20()
{
	unsigned char temp,i;
	DQ = 1;
	for(i=0;i<8;i++)
	{
		EA = 0;
		temp >>= 1;
		DQ = 0;
		delay(1);
		DQ = 1;
		delay(2);
		if(DQ)
			temp |= 0x80;
		delay(45);
		DQ = 1;
		delay(1);
		EA = 1;
	}
	return temp;
}
/*****8:31完成去测试***/
/*****8:53测试完成****/