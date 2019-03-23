#include "define.h"

uchar code table[]= {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf,0xc6,0xff};

uchar led;
uchar pwm;

uchar mode;
uchar ms;
uchar code pwm_f[3] = {2,3,7};

uchar ds_i;
bit ds_f;
uchar nu[8] = {10,1,10,12,0,0,0,0};

void Timer0Init();
void cls_buzz();
void display();

uchar TheKey,Pre,LastKey,Key;
uchar key_t;
bit key_f;
void KeyScan();
void deal_key();

uchar temp;
uchar temp_t;
uchar time_s;
void get_temp();
void init_temp();

bit run_f = 1;
uint left_t;