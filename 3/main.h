#include "define.h"

uchar code table[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf,0xff};

uchar alm;
uchar temp;
uint temp_t;

uchar temp_max,temp_min;
uchar nu[8]={11,11,11,11,11,11,11,11};
uchar ds_i,pwm;
uchar ms;
uchar key_t;key_f;
uchar ReadKey,Dif,Key;
void Timer0Init();
void display();
void RCT();
void cls_buzz();
void get_temp();
void key_scan();
void key_deal();
void Write_24C02(uchar,uchar);
uchar Read_24C02(uchar);
void deal_alm();
void gettemp();
void init_temp();