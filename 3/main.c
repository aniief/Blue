/**********第三届省赛题目***********/
//*********8:20开始*****************/
#include "reg52.h"
#include "define.h"
#include "main.h"
#include "onewire.c"
#include "iic.h"
sfr AUXR = 0x8e;
sfr P4 = 0xC0; 
void main()
{
	cls_buzz();//关闭外设
	init_temp();//温度初始化
	Timer0Init();//定时器0初始化
	while(1)
	{
		get_temp();
	}
}
void Timer0Init()//100us定时器
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0xAE;		//设置定时初值
	TH0 = 0xFB;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;
	EA = 1;
}

void Timer0Int() interrupt 1
{
	ms++;
	if(ms == 7)//低电平占30%
	{
		pwm = 0;
	}
	if(ms == 10)//到达1ms
	{
		temp_t++;//温度转换等待时间
		ms = 0;	//
		
		key_t++;
		if(key_t == 10)//10ms消抖
		{
			key_t = 0;
			key_f = 1;
		}
		display();
		pwm = 1; 	//pwm置1
	}
	deal_alm();	//1ms判断一次温度是否超过上限
}
void init_temp() //上电读取并初始化DS18B20
{
	temp_max = Read_24C02(0x00);//从EEPROM中读取上下限
	nu[0] = temp_max/10;
	nu[1] = temp_max%10;
	temp_min=Read_24C02(0x01);
	nu[2] = temp_min/10;
	nu[3] = temp_min%10;
	Init_DS18B20();	//设置DS18B20转换精度为9位
	Write_DS18B20(0xCC);
	Write_DS18B20(0x4E);
	Write_DS18B20(0x5a);
	Write_DS18B20(0x00);
	Write_DS18B20(0x1f);//0001 1111  9位
	if(temp_max<temp_min)
	{
		temp_max = temp_min;
	}
	temp = temp_min; //当前温度初始化为最低值
}
void deal_alm()//判断温度是否超过设置上下限
{
	if(temp>temp_max)		//超过上限
	{
		P0 = ~pwm;Y4;Y0;	//输出PWM
		alm = 1;			//记为状态1
	}
	else if(temp<temp_min)	//小于下限
	{
		Y5;P0 = 1<<4;Y0;	//打开继电器
		alm = 2;
	}
	else if(alm == 1)		//状态1结束  关闭灯
	{
		alm = 0;
		Y4;P0 = 0xff;Y0;	//结束，PWM置1；
	}
	else if(alm == 2)		//状态2结束  关闭继电器
	{
		Y5;P0 = 0x00;Y0;	//关闭继电器
		alm = 0;
	}
}
void key_scan()
{
	ReadKey = ~KeyPort;
	Dif = ReadKey & (ReadKey^Key);
	Key = ReadKey;
}
void key_deal()//按键处理
{
	if(key_f == 1)
	{
		key_f = 0;
		key_scan();
		if(Dif & 0x01)//S7
		{
			if(temp_max<100)
			{
				temp_max++;
				Write_24C02(0x00,temp_max);
				nu[0] = temp_max/10;
				nu[1] = temp_max%10;
			}
		}
		if(Dif & 0x02)
		{
			if(temp_max>temp_min)
			{
				temp_max--;
				Write_24C02(0x00,temp_max);
				nu[0] = temp_max/10;
				nu[1] = temp_max%10;
			}
		}
		if(Dif & 0x04)
		{
			if(temp_min<temp_max)
			{
				temp_min++;
				Write_24C02(0x01,temp_min);
				nu[2] = temp_min/10;
				nu[3] = temp_min%10;
			}
		}
		if(Dif & 0x08)
		{
			if(temp_min>0)
			{
				temp_min--;
				Write_24C02(0x01,temp_min);
				nu[2] = temp_min/10;
				nu[3] = temp_min%10;
			}
		}
	}
}
void display()
{
	P0 = 0x00;Y6;Y0;
	P0 = 0xff;Y7;P0 = table[nu[ds_i]];Y0;
	P0 = 0x00;Y6;P0 = 0x01<<ds_i;Y0;
	ds_i++;
	ds_i &= 7;
}
void get_temp()
{
	uchar LSB,MSB;
	Init_DS18B20();
	Write_DS18B20(0xCC);
	Write_DS18B20(0x44);
	temp_t=0;
	while(temp_t<100)
	{
		key_deal();
	}
	Init_DS18B20();
	Write_DS18B20(0xCC);
	Write_DS18B20(0xBE);
	LSB = Read_DS18B20();
	MSB = Read_DS18B20();
	temp = 0x0000;
	temp = (MSB<<4)|(LSB>>4);
	//temp += (LSB&0x0f)*0.0625;
	nu[6] = temp/10;
	nu[7] = temp%10;
}


uchar Read_24C02(uchar addr)
{
	uchar tmp;
	//进行一个伪写操作
	IIC_Start();		//IIC总线起始信号					
	IIC_SendByte(0xa0); 	//24C02写设备地址
	IIC_WaitAck();		//等待从机应答	
	IIC_SendByte(addr); 	//内存自己地址
	IIC_WaitAck(); 		//等待从机应答	
	//进行字节读操作
	IIC_Start();		//IIC总线起始信号					
	IIC_SendByte(0xa1); 	//24C02读设备地址
	IIC_WaitAck();		//等待从机应答	
	tmp = IIC_ReadByte();	//读取目标数据
	IIC_Ack(0); 		//产生非应答信号		
	IIC_Stop();		//IIC总线停止信号			
	return tmp;
}

void Write_24C02(unsigned char addr, unsigned char dat)
{
	IIC_Start();		//IIC总线起始信号					
	IIC_SendByte(0xa0); 	//24C02的写设备地址
	IIC_WaitAck();		//等待从机应答	
	IIC_SendByte(addr); 	//内存字节字节
	IIC_WaitAck(); 		//等待从机应答	
	IIC_SendByte(dat); 	//写入目标数据
	IIC_WaitAck();		//等待从机应答	
	IIC_Stop();		//IIC总线停止信号		
}
void cls_buzz()
{
	Y5;P0=0x00;Y0;
}