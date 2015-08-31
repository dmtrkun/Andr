/******************************************************************************/
/* main.h main function prototypes                                         */
/******************************************************************************/
#ifndef _MAIN_H
#define _MAIN_H

#ifdef MAIN_PUB
#define MAINPUB
#else
#define MAINPUB extern
#endif

#define GLOB_CRITSECT_START		{VICIntEnClr  = (1  << 4);};
#define GLOB_CRITSECT_STOP	  {VICIntEnable  = (1  << 4);};                   /* Enable Timer0 Interrupt     */


#define setBit(var, bitnum)   ((var)|=(1<<(bitnum)))
#define resBit(var, bitnum)   ((var)&=~(1<<(bitnum)))
#define clearBit(var, bitnum) ((var)&=~(1<<(bitnum)))
#define testBit(var, bitnum)  ((var)&(1<<(bitnum)))



typedef struct 
{
	unsigned int start;
	unsigned int end;
	unsigned int dst_out;
}
TimerSruct_t;

typedef struct 
{
	TimerSruct_t Timer[10];
	unsigned int check;
}SetupImg_t;

#ifdef MAIN_PUB
char *out_names[] =
{
"LIGHT1",
"LIGHT2",
"CO2   ",
"AIR   ",
"FERT  ",
"TEMP  ",
};

#else
extern char *out_names[];
#endif



MAINPUB __align(8) SetupImg_t SetupImg;
MAINPUB char str[17];
MAINPUB RTCTime set_time, current_time;
MAINPUB uchar relay_status;
MAINPUB uchar test_flg, test_cntr;
MAINPUB void set_defaults(void);
MAINPUB void CPU_LED_Toggle (void);




#define BATTERY_RAM_BASE_ADDRESS 0xE0084000

/* ALLOCATION */

#define BSR_para1 (*(volatile unsigned long*)(BATTERY_RAM_BASE_ADDRESS + 0x0000))
#define BSR_para2 (*(volatile unsigned long*)(BATTERY_RAM_BASE_ADDRESS + 0x0004))
#define BSR_setup_img_t (*(volatile SetupImg_t*)(BATTERY_RAM_BASE_ADDRESS + 0x0008))

#endif // _MAIN_H

/******************************************************************************/

