#include "define.h"
sfr AUXR = 0x8e;
sfr T2H  =  0xd6;
sfr T2L  =  0xd7;
uchar code table[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf};
uchar nu[8] = {10,10,10,10,10,10,10,10};

void cls_buzz();
uchar ds_i;
void display();
void InitAIN();
void GetAIN();
uchar whats;
void Timer0Init(void);