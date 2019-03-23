#include "reg52.h"
#include "iic.c"
typedef unsigned int uint;
typedef unsigned char uchar;
#define Y0 P2 &= 0x1f
#define Y5 P2 = (P2&0x1f)|0xa0
#define Y6 P2 = (P2&0x1f)|0xc0
#define Y7 P2 = (P2&0x1f)|0xe0
uchar code table[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf};
uchar nu[8];
void delay(uint);
void display();
void Write_24C02(uchar,uchar);
uchar Read_24C02(uchar);
uchar a,b,c,d,e;
uchar ds_i;
void main()
{
	P0=0x00;Y5;Y0;
	/*nu[0] = Read_24C02(0x01);
	nu[1] = Read_24C02(0x02);
	nu[2] = Read_24C02(0x03);
	nu[3] = Read_24C02(0x04);
	nu[4] = Read_24C02(0x05);*/
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(0x01);
	IIC_WaitAck();
	IIC_Stop();

	IIC_Start();
	IIC_SendByte(0xa1);
	IIC_WaitAck();
	nu[0] = IIC_RecByte();
	IIC_SendAck(0);
	nu[1] = IIC_RecByte();
	IIC_SendAck(0);
	nu[2] = IIC_RecByte();
	IIC_SendAck(0);
	nu[3] = IIC_RecByte();
	IIC_SendAck(1);
	IIC_Stop();
	nu[0]++;nu[1]++;nu[2]++;nu[3]++;nu[4]++;
	nu[0]%=10;nu[1]%=10;nu[2]%=10;nu[3]%=10;nu[4]%=10;
	/*Write_24C02(0x01,nu[0]);
	//delay(1000);
	Write_24C02(0x02,nu[1]);
	//delay(1000);
	Write_24C02(0x03,nu[2]);
	//delay(1000);
	Write_24C02(0x04,nu[3]);
	//delay(1000);
	Write_24C02(0x05,nu[4]);*/
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(0x01);
	IIC_WaitAck();
	IIC_SendByte(nu[0]);
	IIC_WaitAck();
	IIC_SendByte(nu[1]);
	IIC_WaitAck();
	IIC_SendByte(nu[2]);
	IIC_WaitAck();
	IIC_SendByte(nu[3]);
	IIC_WaitAck();
	IIC_Stop();
	while(1)
	{
		display();
	}
}
void display()
{
	Y6;P0=0x00;Y0;
	Y7;P0 = table[nu[ds_i]];Y0;
	Y6;P0=0x01<<ds_i;Y0;
	ds_i++;
	ds_i &= 7;
	delay(500);
}
void delay(uint t)
{
	while(t--);
}
void Write_24C02(uchar addr,uchar dat)//Write 1 Byte 
{
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(addr);
	IIC_WaitAck();
	IIC_SendByte(dat);
	IIC_WaitAck();
	IIC_Stop();
}
uchar Read_24C02(uchar addr)
{
	uchar dat;
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(addr);
	IIC_WaitAck();
	IIC_Stop();
	
	IIC_Start();
	IIC_SendByte(0xa1);
	IIC_WaitAck();
	dat = IIC_RecByte();
	IIC_SendAck(1);
	IIC_Stop();
	return dat;
}