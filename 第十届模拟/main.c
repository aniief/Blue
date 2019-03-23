#include "main.h"
#include "iic.c"
#include "ds1302.c"
void main()
{
	mode = 1;
	SysInit();
	
	while(1)
	{
		SysRun();
	}
}

void SysRun()
{
	KeyDeal();
	if(mode == TIME)
	{
		GetTime();
	}
	else if(mode == VOLTE)
	{
		if(volt_f == 1)
		{
			volt_f = 0;
			DealVolt();
		}
	}
}
void Timer1Int() interrupt 3
{

		ms++;	//  Fre  
		if(ms == 1000)
		{
			TR0 = 0;
			ms = 0;
			if(mode == 3)
			{
				f=((TH0<<8)|TL0);
				if(fren_set == 1)
				{
					f = 100000/f;
				}
				
				nu[3] = f/10000;
				nu[4] = f%10000/1000;
				nu[5] = f%1000/100;
				nu[6] = f%100/10;
				nu[7] = f%10;
			}
			f=0;
			TH0 = 0x00;
			TL0 = 0x00;
			TR0 = 1;
		
	}
	key_t++;
	if(key_t == 10)
	{
		key_f = 1;
		key_t = 0;
	}
	volt_t++;
	if(volt_t == 500)
	{
		volt_t = 0;
		volt_f = 1;
	}
	ds_t++;
	if(ds_t == 1000)
	{
		ds_f = ~ds_f;
		ds_t = 0;
	}
	display_pro();
}

void SysInit()
{
	cls_buzz();
	InitVolt();
	LoadEvent();
	InitTime();
	Timer1Init();
	Timer0Init();
}
void InitVolt()
{
	IIC_Start();
	IIC_SendByte(0x90);
	IIC_WaitAck();
	IIC_SendByte(0x03);
	IIC_WaitAck();
	IIC_Stop();
}
uchar GetVolt()
{
	uchar volt_temp;
	IIC_Start();
	IIC_SendByte(0x91);
	IIC_WaitAck();
	volt_temp = IIC_RecByte();
	IIC_SendAck(1);
	IIC_Stop();
	return volt_temp;
}
void DealVolt()
{
	lastvolt = thevolt;
	thevolt = GetVolt();
	thevolt = thevolt*10/51;
	nu[2] = thevolt/10;
	nu[3] = thevolt%10;
	if((lastvolt>=VL)&&(thevolt<VL))
	{
		event = 0;
		nu[7] = 0;
		EA = 0;
		event_s = Read_Ds1302_Byte(0x81);
		event_m = Read_Ds1302_Byte(0x83);
		event_h = Read_Ds1302_Byte(0x85);
		EA = 1;
		event_s = (event_s>>4)*10 + (event_s&0x0f);// + > &  must have "()"
		event_m = (event_m>>4)*10 + (event_m&0x0f);
		event_h = (event_h>>4)*10 + (event_h&0x0f);
		Write();
		/*Write_24C02(0x00,event_h);
		Write_24C02(0x01,event_m);
		Write_24C02(0x02,event_s);
		Write_24C02(0x03,event);*/
	}
	else if((lastvolt<=VH)&&(thevolt>VH))
	{
		event = 1;
		nu[7] = 1;
		EA = 0;
		event_s = Read_Ds1302_Byte(0x81);
		event_m = Read_Ds1302_Byte(0x83);
		event_h = Read_Ds1302_Byte(0x85);
		EA = 1;
		event_s = (event_s>>4)*10 + (event_s&0x0f);// + > &  must have "()"
		event_m = (event_m>>4)*10 + (event_m&0x0f);
		event_h = (event_h>>4)*10 + (event_h&0x0f);
		Write();
		/*Write_24C02(0x00,event_h);
		Write_24C02(0x01,event_m);
		Write_24C02(0x02,event_s);
		Write_24C02(0x03,event);*/
	}
}
void InitTime()
{
	//if(times != 0x55)
	//{
		Write_Ds1302_Byte(0x8e,0x00);
		Write_Ds1302_Byte(0x80,0x51);
		Write_Ds1302_Byte(0x82,0x59);
		Write_Ds1302_Byte(0x84,0x23);
	//}
}
void GetTime()
{
	uchar temp_s,temp_m,temp_h;
	temp_s = Read_Ds1302_Byte(0x81);
	temp_m = Read_Ds1302_Byte(0x83);
	temp_h = Read_Ds1302_Byte(0x85);
	time_s = (temp_s>>4)*10 + (temp_s&0x0f);// + > &  must have "()"
	time_m = (temp_m>>4)*10 + (temp_m&0x0f);
	time_h = (temp_h>>4)*10 + (temp_h&0x0f);
	nu[0] = time_h/10;nu[1] = time_h%10;
	nu[3] = time_m/10;nu[4] = time_m%10;
	nu[6] = time_s/10;nu[7] = time_s%10;
	nu[2] = nu[5] = 10;
	/*nu[7] = temp_s&0x0f;
	nu[6] = temp_s>>4; 
	nu[4] = temp_m&0x0f;
	nu[3] = temp_m>>4;
	nu[1] = temp_h&0x0f;
	nu[0] = temp_h>>4;*/
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
	return dat;
}
void Write()
{
	EA = 0;
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(0x00);
	IIC_WaitAck();
	IIC_SendByte(event_h);
	IIC_WaitAck();
	IIC_SendByte(event_m);
	IIC_WaitAck();
	IIC_SendByte(event_s);
	IIC_WaitAck();
	IIC_SendByte(event);
	IIC_WaitAck();
	IIC_Stop();
	EA = 1;
}
void Write_24C02(uchar addr,uchar dat)
{
	EA = 0;
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(addr);
	IIC_WaitAck();
	IIC_SendByte(dat);
	IIC_WaitAck();
	IIC_Stop();
	EA = 1;
}
void LoadEvent()
{
	event_h = Read_24C02(0x00);
	event_m = Read_24C02(0x01);
	event_s = Read_24C02(0x02);
	event = Read_24C02(0x03);
	VL = Read_24C02(0x04);
	VH = Read_24C02(0x05);
	times = Read_24C02(0x08);
	if(times != 0x55)//第一次上电
	{
		VL = 10;
		VH = 20;
	}
	Write_24C02(0x08,0x55);
}

void Timer0Init()//Timer counter
{
	TMOD &= 0xF0;
	TMOD |= 0x04;
	TH0 = 0x00;
	TL0 = 0x00;
}

void cls_buzz()
{
	Y5;P0=0x00;Y0;
	Y4;P0=0xff;Y0;
}
void Timer1Init(void)		//1毫秒@11.0592MHz
{
	AUXR |= 0x50;		//定时器时钟1T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0xCD;		//设置定时初值
	TH1 = 0xD4;		//设置定时初值
	TF1 = 0;		//清除TF0标志
	TR1 = 1;		//定时器0开始计时
	EA = 1;
	ET1 = 1;
}
void display_pro()
{
	if(mode == TIME)
	{
		if(time_set == 1)
		{
			if(ds_i<2)
			{
				if(ds_f == 1)
					display();
				else 
				{
					P0=0x00;Y6;Y0;
					ds_i++;
					ds_i &= 7;
				}
			}
			else display();
		}
		else if(time_set == 2)
		{
			if((ds_i<5)&&(ds_i>2))
			{
				if(ds_f == 1)
					display();
				else 
				{
					P0=0x00;Y6;Y0;
					ds_i++;
					ds_i &= 7;
				}
			}
			else display();
		}
		else if(time_set == 3)
		{
			if(ds_i>5)
			{
				if(ds_f == 1)
					display();
				else 
				{
					P0=0x00;Y6;Y0;
					ds_i++;
					ds_i &= 7;
				}
			}
			else display();
		}
		else display();
	}
	else if(mode == VOLTE)
	{
		if(volt_set == 1)
		{
			if(ds_i<4)
			{
				if(ds_f == 1)
					display();
				else 
				{
					P0=0x00;Y6;Y0;
					ds_i++;
					ds_i &= 7;
				}
			}
			else display();
		}
		else if(volt_set == 2)
		{
			if(ds_i>3)
			{
				if(ds_f == 1)
					display();
				else 
				{
					P0=0x00;Y6;Y0;
					ds_i++;
					ds_i &= 7;
				}
			}
			else display();
		}
		else display();
	}		
	else if(mode == FREN)
	{
		display();
	}
	else if(mode == CHECK)
	{
		display();
	}
}

void display()
{

	P0 = 0x00;Y6;Y0;
	P0 = table[nu[ds_i]];Y7;Y0;
	P0 = 0x01<<ds_i;Y6;Y0;
	ds_i++;
	ds_i &= 7;
}

void KeyDeal()
{
	if(key_f == 1)
	{
		key_t = 0;
		key_f = 0;
		KeyValue = ScanKey();
		if(KeyValue != 88)
		{
			//S7--TIME
			if(KeyValue == 7)//S7   TIME_DISPLAY
			{
				mode = TIME;
				time_set = 0;
				nu[0] = time_h/10;nu[1] = time_h%10;
				nu[3] = time_m/10;nu[4] = time_m%10;
				nu[6] = time_s/10;nu[7] = time_s%10;
				nu[2] = nu[5] = 10;
			}
			if(mode == TIME)//TIME'setting
			{
				if(KeyValue == 4)//chang setting
				{
					time_set++;
					time_set %= 4;//1--H;2--M;3--S;
					if(time_set == 0)
					{
						time_set ++;
					}
				}
				if((KeyValue == 11)&(time_set!=0))// ++h/m/s
				{
					if(time_set == 1)// set h
					{
						time_h++;
						if(time_h==24)
						{
							time_h = 0;
						}
						nu[0] = time_h/10;nu[1] = time_h%10;
						Write_Ds1302_Byte(0x84, (time_h/10)*16+time_h%10 );
					}
					else if(time_set == 2)
					{
						time_m++;
						if(time_m == 60)
						{
							time_m = 0;
						}
						nu[3] = time_m/10;nu[4] = time_m%10;
						Write_Ds1302_Byte(0x82,(time_m/10)*16+time_m%10 );
					}
					else if(time_set == 3)
					{
						time_s++;
						if(time_s == 60)
						{
							time_s = 0;
						}
						nu[6] = time_s/10;nu[7] = time_s%10;
						Write_Ds1302_Byte(0x80,(time_s/10)*16+time_s%10 );
					}
				}
				if((KeyValue == 10)&(time_set!=0))// --h/m/s
				{
					if(time_set == 1)// set h
					{
						if(time_h == 0)
						{
							time_h = 24;
						}
						time_h--;
						nu[0] = time_h/10;nu[1] = time_h%10;
						Write_Ds1302_Byte(0x84, (time_h/10)*16+time_h%10 );
					}
					else if(time_set == 2)
					{
						
						if(time_m == 0)
						{
							time_m = 60;
						}
						time_m--;
						nu[3] = time_m/10;nu[4] = time_m%10;
						Write_Ds1302_Byte(0x82,(time_m/10)*16+time_m%10 );
					}
					else if(time_set == 3)
					{
						
						if(time_s == 0)
						{
							time_s = 60;
						}
						time_s--;
						nu[6] = time_s/10;nu[7] = time_s%10;
						Write_Ds1302_Byte(0x80,(time_s/10)*16+time_s%10 );
					}
				}
			}
			
			//S6--VOLTE
			if(KeyValue == 6)
			{
				lastvolt = GetVolt();
				mode = VOLTE;
				if(volt_set != 0)
				{
					VH = VH_temp;
					VL = VL_temp;
					EA = 0;
					IIC_Start();
					IIC_SendByte(0xa0);
					IIC_WaitAck();
					IIC_SendByte(0x04);
					IIC_WaitAck();
					IIC_SendByte(VL);
					IIC_WaitAck();
					IIC_SendByte(VH);
					IIC_Stop();
					EA = 1;
				}
				volt_set = 0;
				nu[0] = VH/10;nu[1] = VH%10;
				nu[2] = 0;nu[3] = 0;nu[6] = 0;nu[7] = event;
				nu[4] = VL/10;nu[5] = VL%10;
			}
			if(mode == VOLTE)
			{
				if(KeyValue == 4)
				{
					if(volt_set == 0)
					{
						VL_temp = VL;
						VH_temp = VH;
					}
					volt_set++;
					volt_set %= 3;
					if(volt_set == 0)
					{
						volt_set++;
					}
				}
				else if(KeyValue == 11)//++ VH/VL
				{
					if(volt_set == 1)//VH++
					{
						if(VH_temp < 46)
						{
							VH_temp += 5;
							nu[0] = VH_temp/10;
							nu[1] = VH_temp%10;
						}
					}
					else if(volt_set == 2)//VL++
					{
						if((VL_temp + 5) < VH_temp)
						{
							VL_temp += 5;
							nu[4] = VL_temp/10;
							nu[5] = VL_temp%10;
						}
					}
				}
				else if(KeyValue == 10)//-- VH/Vl
				{
					if(volt_set == 1)//VH--
					{
						if((VH_temp-5) > VL_temp)
						{
							VH_temp -= 5;
							nu[0] = VH_temp/10;
							nu[1] = VH_temp%10;
						}
					}
					else if(volt_set == 2)//VL--
					{
						if(VL_temp>4)
						{
							VL_temp -= 5;
							nu[4] = VL_temp/10;
							nu[5] = VL_temp%10;
						}
					}
				}
			}
			//S5--FREN
			if(KeyValue == 5)
			{
				mode = FREN;
				nu[0] = nu[2] = 10;
				nu[1] = 2;
				nu[7] = 8;
			}
			if(mode == FREN)
			{
				if(KeyValue == 4)
				{
					fren_set++;
					fren_set %= 2;
				}
			}
			
			//S9   CHECK
			if(KeyValue == 9)
			{
				check_set = 0;
				mode = CHECK;
				nu[0] = nu[1] = nu[2] = nu[3] = nu[4] = nu[5] = 11;
				nu[6] = 0;nu[7] = event;
			}
			if(mode == CHECK)
			{
				if(KeyValue == 4)
				{
					if(check_set == 1)
					{
						check_set = 0;
						nu[0] = event_h/10;nu[1] = event_h%10;
						nu[3] = event_m/10;nu[4] = event_m%10;
						nu[6] = event_s/10;nu[7] = event_s%10;
						nu[2] = nu[5] = 10;
					}
					else if(check_set == 0)
					{
						check_set = 1;
						nu[0] = nu[1] = nu[2] = nu[3] = nu[4] = nu[5] = 11;
						nu[6] = 0;nu[7] = event;
					}
				}
			}
		}
	}
}

uchar ScanKey()
{
		C1 = 0;C2 = 1;
		TheKey1 = ~KeyPort;
		PreKey1 = TheKey1&(TheKey1^LastKey1);
		LastKey1 = TheKey1;
		if(PreKey1&0x01)
		{
			return 7;
		}
		else if(PreKey1&0x02)
		{
			return 6;
		}
		else if(PreKey1&0x04)
		{
			return 5;
		}
		else if(PreKey1&0x08)
		{
			return 4;
		}
		
		C1 = 1;C2 = 0;
		TheKey2 = ~KeyPort;
		PreKey2 = TheKey2&(TheKey2^LastKey2);
		LastKey2 = TheKey2;
		if(PreKey2&0x01)
		{
			return 11;
		}
		else if(PreKey2&0x02)
		{
			return 10;
		}
		else if(PreKey2&0x04)
		{
			return 9;
		}
		else if(PreKey2&0x08)
		{
			return 8;
		}
		return 88;
}