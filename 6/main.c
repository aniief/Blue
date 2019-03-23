#include "stc15.h"
#include "onewire.c"
#include "main.h"
void main()
{
	sys_init();//系统初始化，关闭无关外设和初始化DS18B20
	Timer0Init();//定时器初始化
	while(1)
	{
		get_temp();//读取温度，按键处理嵌入到等待温度转换的时间里
	}
}
void Timer0Int() interrupt 1 
{
	key_t++;		//按键
	if(key_t == 10)
	{
		key_t = 0;
		key_f = 1;
	}
	ms++;//ms计时
	if(temp_zone == 0)//温度在区间0
	{
		if(ms >= 800)	//0.8s闪烁
		{
			ms = 0 ;
			led = ~led;
		}
	}
	if(temp_zone == 1)//温度在区间1
	{
		if(ms >= 400)//0.4s闪烁
		{
			ms = 0 ;
			led = ~led;
		}
	}
	if(temp_zone == 2)//温度在区间2
	{
		if(ms >= 200)//0.2s闪烁
		{
			ms = 0 ;
			led = ~led;
		}
	}
	if(Wr == 0)//温度上下限的输入是否错误
	{
		if(set == 0)
			alm_deal();//报警，控制led等和继电器
	}
	temp_t++;//温度转换等待时间
	display();//显示
}
void alm_deal()//灯光控制，放中断里
{
	P0 = 0xfd|led;Y4;Y0;
	if(temp_zone == 0)
	{
		P0 = 0x00;Y5;Y0;
	}
	else if(temp_zone == 1)
	{
		P0 = 0x00;Y5;Y0;
	}
	else if(temp_zone == 2)
	{
		P0 = (1<<4);Y5;Y0;
	}
}
void display()
{
	P0 = 0x00;Y6;Y0;
	P0 = table[nu[ds_i]];Y7;Y0;
	P0 = 0x01<<ds_i;Y6;Y0;
	ds_i++;
	ds_i %= 8;
}
void get_temp()
{
	uchar LSB,MSB;
	Init_DS18B20();
	Write_DS18B20(0xCC);
	Write_DS18B20(0x44);
	temp_t = 0;
	while(temp_t < 94)//等待94ms，温度转换需要
	{
		key_deal();
	}
	Init_DS18B20();
	Write_DS18B20(0xCC);
	Write_DS18B20(0xBE);
	LSB = Read_DS18B20();
	MSB = Read_DS18B20();
	temp = (MSB<<4)|(LSB>>4);
	if((LSB<<4) == 0x80)
		temp ++;
	if(set ==0) // 未进入设置模式下才可以更新nu[]的值，不然在设置模式下无法正常显示
	{
		nu[6] = temp/10;
		nu[7] = temp%10;
		if(temp>temp_max)	//判断温度所属区间
			temp_zone = 2;
		else if(temp<temp_min)
			temp_zone = 0;
		else 
			temp_zone = 1;
		nu[1] = temp_zone;
	}
}
void key_deal()
{
	if(key_f == 1)
	{
		key_f = 0;
		Kb = KeyScan();
		if(Kb != 66)
		{
			if(set == 0)//当前在温度显示界面
			{
				if(Kb == 10)	//按下“设置”进入温度设置界面
				{
					Y4;P0 = 0xff;Y0;//设置模式下灯光不应该在闪
					set = 1;
					nu[0]=10;nu[1]=11;nu[2]=11;nu[3]=11;	//清屏，等待输入工作参数
					nu[4]=11;nu[5]=10;nu[6]=11;nu[7]=11;
				}
			}
			else if(set == 1)//当前在温度设置界面
			{
				if(Kb == 10)//按下“设置”尝试保存数据
				{
					if(set_i == 4)//设置完毕
					{
						if(temp_max>=temp_min)//工作参数正确---可以保存数据
						{
							Y4;P0 = 0xff;Y0;Wr = 0;
							set = 0;
							set_i = 0;
							nu[0]=10;nu[1]=temp_zone;nu[2]=10;nu[3]=11;
							nu[4]=11;nu[5]=11;nu[6]=temp/10;nu[7]=temp%10;
						}
						else//工作参数设置错误
						{
							Wr = 1;//警示，防止led按照区间条件闪烁
							Y4;P0 = ~(1<<1);Y0;
							set_i=0;
							nu[0]=10;nu[1]=11;nu[2]=11;nu[3]=11;//清屏，等待重新输入
							nu[4]=11;nu[5]=10;nu[6]=11;nu[7]=11;
						}
					}
				}
				else//按下的不是“设置”
				{
					if(Kb == 11)//重置
					{
						set_i = 0;
						nu[0]=10;nu[1]=11;nu[2]=11;nu[3]=11;//清屏，等待重新输入
						nu[4]=11;nu[5]=10;nu[6]=11;nu[7]=11;
					}
					else if(set_i == 0)	//输入第一个数字
					{
						temp_max = Kb*10;//max的十位
						nu[1]=Kb;
						set_i++;
					}
					else if(set_i == 1)//输入第二个数字
					{
						temp_max += Kb;//max的个位
						nu[2]=Kb;
						set_i++;
					}
					else if(set_i == 2)//输入第三个数字
					{
						temp_min = Kb*10;
						nu[6]=Kb;
						set_i++;
					}
					else if(set_i == 3)//输入第四个数字
					{
						temp_min += Kb;
						nu[7]=Kb;
						set_i++;
					}
				}
			}
		}
	}
}
uchar KeyScan()
{
	R1 = 0;R2 = 1;R3 = 1;R4 = 1;
	readkey = ~KeyPort;
	Pre1 = readkey&(readkey^LastKey1);
	LastKey1 = readkey;
	if(Pre1&0x08)
		return 9;
	if(Pre1&0x04)
		return 6;
	if(Pre1&0x02)
		return 3;
	if(Pre1&0x01)
		return 0;
	
	R1 = 1;R2 = 0;R3 = 1;R4 = 1;
	readkey = ~KeyPort;
	Pre2 = readkey&(readkey^LastKey2);
	LastKey2 = readkey;
	if(Pre2&0x08)
		return 10;//shezhi
	if(Pre2&0x04)
		return 7;
	if(Pre2&0x02)
		return 4;
	if(Pre2&0x01)
		return 1;
	
	R1 = 1;R2 = 1;R3 = 0;R4 = 1;
	readkey = ~KeyPort;
	Pre3 = readkey&(readkey^LastKey3);
	LastKey3 = readkey;
	if(Pre3&0x08)
		return 11;//clr
	if(Pre3&0x04)
		return 8;
	if(Pre3&0x02)
		return 5;
	if(Pre3&0x01)
		return 2;
	
	R1 = 1;R2 = 1;R3 = 1;R4 = 0;
	readkey = ~KeyPort;
	Pre4 = readkey&(readkey^LastKey4);
	LastKey4 = readkey;
	if(Pre4&0x08)
		return 66;
	if(Pre4&0x04)
		return 66;
	if(Pre4&0x02)
		return 66;
	if(Pre4&0x01)
		return 66;
	return 66;
}
void Timer0Init(void)		//1毫秒@11.0592MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0xCD;		//设置定时初值
	TH0 = 0xD4;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	EA = 1;
	ET0 = 1;
}

void sys_init()
{
	Y4;P0 = 0xff;Y0;
	Y5;P0 = 0x00;Y0;
	Init_Temp();
}
void Init_Temp()
{
	Init_DS18B20();
	Write_DS18B20(0xCC);
	Write_DS18B20(0x4E);
	Write_DS18B20(100);
	Write_DS18B20(0);
	Write_DS18B20(0x1F);
	temp_max = 30;
	temp_min = 20;
}