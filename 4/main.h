#include "define.h"
uchar code table[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf,0xff};
uchar nu[8];
sfr AUXR = 0x8e;//
bit counter_f = 0;//计费标志位
uchar ms;	//ms计数
uint money;//价格
uint water;//出水体积
bit cont_f;//
uchar ds_i;
void display();
void sys_init();
void Timer0Init();

//key
#define KeyPort P3
uchar key_t;
bit key_f;
uchar readkey,Pre,LastKey;
void KeyScan();
void key_deal();
//温度
uchar temp;
void init_PCF8591();
void get_PCF8591();
//灯亮灭标志
bit led_f;