#include "define.h"
#define KeyPort P3
uchar code table[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf,0xff};
uchar nu[8] = {10,10,10,11,11,11,10,10};

uchar temp,temp_max,temp_min,temp_zone;
uint temp_t;
uchar ds_i,Wr;
uint ms;
uchar led = 0xff;

uchar set_i,set;
uchar readkey,Kb;
uchar key_f,key_t;
uchar Pre1,Pre2,Pre3,Pre4,LastKey1,LastKey2,LastKey3,LastKey4;
sbit R1 = P4^4;
sbit R2 = P4^2;
sbit R3 = P3^5;
sbit R4 = P3^4;
uchar KeyScan();

void key_deal();
void Timer0Init(void);
void sys_init();
void Init_Temp();
void display();
void get_temp();
void alm_deal();