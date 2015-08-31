/******************************************************************************/
/* BLINKY.C: LED Flasher                                                      */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005-2006 Keil Software. All rights reserved.                */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/
#define MAIN_PUB

/*
 * 	PLL clock = (2 x 8MHz * M)/N = 176Mhz
 *  N = 1
 *  M = 11
 * 	Clk =
 *
 *
 * */



                  
#include <stdio.h>
#include <LPC23xx.H>                    /* LPC23xx definitions                */
#include "include.h"


#define COUNTER1000Hz	36000L
#define COUNTER100Hz	360000L
#define COUNTER10Hz		3600000L
#define COUNTER1Hz		36000000L

#define MIN_FREQ 		4294967294L  //0.016763806350884868926780702977805 Hz

#define COUNTER_0_02HZ	3600000000L

#define COUNTER50000Hz	(36000L / 50L)

void main_loop(void);
__irq void T2_IRQHandler(void);



extern volatile BYTE *WriteBlock; /* treat WriteBlock as a constant address */
extern volatile BYTE *ReadBlock; /* treat ReadBlock as a constant address */
extern volatile DWORD MCI_Block_End_Flag;

unsigned int freq_out = 0;
char name[16];
unsigned int freq_out_real = 0;
unsigned int num_tics_correct = 0;
unsigned int num_tics_correct_counter = 0;
unsigned long cycle_counter=0;
unsigned long num_tics;
unsigned long timer_cntr;
uchar runStatus = 0;
uchar runMode = 1;
uchar Pause = 0;


unsigned int freq_table[] =
{
		1000,
		32400,
		52800,
		1500,
		133500,
		48000,
		172500, 64500, 134200,
		66200,
		153400, 141300, 135100,
		53700, 153700,
		83500,
		63500,
		76300,
		65400,
		75100,
		69600,
		62500,
		76400,
		67600, 1520,
		535, 734400,
		264200,
		1000000, 420000,
		200, 65700,
		9700,
		700,
		1350,
		1520,

		32400, 52800, 1500, 30400, 9500, 1500, 919, 600,
		2000000,
		6650, 5930,
		1000000, 64600, 14600, 105,
		35000,36000,
		152400,
		143400,

		46500,
};

typedef struct t_FREQ_TABLE
{
	int real_freq;
	const char name[16];
}FREQ_TABLE;

FREQ_TABLE struct_freq_table_1[] =
{
		{1000, "10.00"},
//		{32400, "324.00"},
//		{52800, "528.00"},
		{1500,"15.00"},
//		{133500,"1335.00"},

		{47999,"480.00"},
		{172503,"1725.00"},
		{64499,"645.00"},
		{134201,"1342.00"},
		{66199,"662.00"},
		{153402,"1534.00"},
		{141301,"1413.00"},
		{135101, "1351.00"},
		{53699,"537.00"},
		{153702,"1537.00"},
		{83500,"835.00"},
		{63499,"635.00"},
		{76299,"763.00"},
		{65399,"654.00"},
		{75100,"751.00"},
		{69599,"696.00"},
		{62499,"625.00"},
		{76399,"764.00"},
		{67599,"676.00"},
		{1520,"15.20"},
		{535,"5.35"},
		{734530,"7344.00"},
		{264220,"2642.00"},
		{1000250,"10000.00"},
		{420035,"4200.00"},
		{200,"2.00"},
		{65699, "657.00"},
		{9700,"97.00"},
		{700,"7.00"},
		{1350,"13.50"},
		{1520,"15.20"},
		{32400,"324.00"},
		{52799,"528.00"},
		{1500,"15.00"},
		{30400,"304.00"},
		{9500,"95.00"},
		{1500,"15.00"},
		{919,"9.19"},
		{600,"6.00"},
		{2000011,"20000.00"},
		{6650,"66.50"},
		{5930,"59.30"},
		{1000250,"10000.00"},
		{64599,"646.00"},
		{14600,"146.00"},
		{105,"1.05"},
		{35000,"350.00"},
		{36000,"360.00"},
		{152402,"1524.00"},
		{143401,"1434.00"},
		{46500,"465.00"},
		{93400,"934.00"},
		{80200,"802.00"},
};
FREQ_TABLE struct_freq_table[] =
{
		{1000, "10.00"},
		{32400, "324.00"},
		{52800, "528.00"},
		{1500,"15.00"},
		{14,"0.14"},
		{62,"0.62"},
		{85,"0.85"},
		{507,"5.07"},
		{1285,"12.85"},
		{45372, "453.72"},
		{51509, "515.09"},
		{68481, "684.81"},
		{71281, "712.81"},
		{99341, "993.41"},
		{46500, "465.00"},
};


/* Function that initializes LEDs                                             */
void LED_Init(void) {
  PINSEL10 = 0;                         /* Disable ETM interface, enable LEDs */
  IODIR0  |= 0x00380000;                /* P0.19..21 defined as Outputs         */
//  IOMASK0 = 0x00000000;
  IODIR1  |= 0x00000200;                /* P1.9 defined as Output         */
}

void CPU_LED_Toggle (void) {

	if(IOPIN1 & (1 << 9))
		IOCLR1 = (1 << 9);
	else
		IOSET1 = (1 << 9);
}

/* Function that turns on requested LED                                       */
void LED_On (unsigned int num) {
  IOSET0 = ((1 << num)<<19);
}

/* Function that turns off requested LED                                      */
void LED_Off (unsigned int num) {
  IOCLR0 = ((1 << num)<<19);
}

/* Function that outputs value to LEDs                                        */
void LED_Out(unsigned int value) {
  IOCLR0 = 0x00380000;                       /* Turn off all LEDs                  */
  IOSET0 = ((value<<19) & 0x00380000);       /* Turn on requested LEDs             */
}


/* Function for displaying bargraph on the LCD display                        */
void Disp_Bargraph(int pos_x, int pos_y, int value) {
  int i;

  set_cursor (pos_x, pos_y);
  for (i = 0; i < 16; i++)  {
    if (value > 5)  {
      lcd_putchar (0x05);
      value -= 5;
    }  else  {
      lcd_putchar (value);
      value = 0;
    }
  }
}

/* Import external IRQ handlers from IRQ.c file                               */
//extern __irq void T0_IRQHandler  (void);
//extern __irq void ADC_IRQHandler (void);

/* Import external functions from Serial.c file                               */
extern       void init_serial    (void);

//extern short AD_last;
//extern unsigned char clock_1s;


int main (void) {
  int i;
  unsigned int time_s;
  short AD_old, AD_value, AD_print;
	unsigned long *src;
	unsigned long *dst;
  
  relay_status = 0;
  LED_Init();                           /* LED Initialization                 */

  /* Enable and setup timer interrupt, start timer                            */
  T0MR0         = 17999;                       /* 1msec = 18000-1 at 18.0 MHz (72MHz / 4) */
  T0MCR         = 3;                           /* Interrupt and Reset on MR0  */
  T0TCR         = 1;                           /* Timer0 Enable               */
  VICVectAddr4  = (unsigned long)T0_IRQHandler;/* Set Interrupt Vector        */
  VICVectCntl4  = 15;                          /* use it for Timer0 Interrupt */
  VICIntEnable  = (1  << 4);                   /* Enable Timer0 Interrupt     */

  /* Power enable, Setup pin, enable and setup AD converter interrupt         */
  //PCONP        |= (1 << 12);                   /* Enable power to AD block    */
  //PINSEL1       = 0x4000;                      /* AD0.0 pin function select   */
  //AD0INTEN      = (1 <<  0);                   /* CH0 enable interrupt        */
  //AD0CR         = 0x00200301;                  /* Power up, PCLK/4, sel AD0.0 */
  //VICVectAddr18 = (unsigned long)ADC_IRQHandler;/* Set Interrupt Vector       */
  //VICVectCntl18 = 14;                          /* use it for ADC Interrupt    */
  //VICIntEnable  = (1  << 18);                  /* Enable ADC Interrupt        */

//	SSP1Init();
	init_serial();                               /* Init UART                   */
	rtc_init();
	init_keypad();



/* Backlighjt Control*/
//p1.25 backlight - MAT1

	  T1MR1         = 17999;                       /* 1msec = 18000-1 at 18.0 MHz (72MHz / 4) */
	  T1MCR         = 0x00000010;                  /* Reset on MR1  */
	  T1TCR         = 1;                           /* Timer1 Enable               */
	  T1EMR = 0x000000F3;  //Toggle MAT1[0],MAT1[1] External Match bit/output
	  PINSEL3 |= 0x000C0000;
//	  VICVectAddr4  = (unsigned long)T1_IRQHandler;/* Set Interrupt Vector        */
//	  VICVectCntl4  = 15;                          /* use it for Timer0 Interrupt */
//	  VICIntEnable  = (1  << 4);                   /* Enable Timer0 Interrupt     */



//p0.6 MAT2.0  --SSEL1
//	  P3.25 R73 - MAT0[0]

//	  /CS_ADC1 P1,29 MAT0[1]
//	  T0MR1         = 17999;                       /* 1msec = 18000-1 at 18.0 MHz (72MHz / 4) */
//	  T0MCR |= 0x00000010; //Reset on MR1
//	  T0EMR = 0x000000F3;  //Toggle MAT0[0],MAT0[1] External Match bit/output


//p0.6 MAT2.0  --SSEL1
	  PCONP        |= (1 << 22);                   /* Enable power to TIMER2 block    */
	  T2MR0         = COUNTER100Hz;                       /*  for 1kHz (72MHz clock)*/
	  T2MCR         = 3;                           /* Interrupt and Reset on MR0  */
	  T2TCR         = 0;                           /* Timer2 Disable               */
	  VICVectAddr26  = (unsigned long)T2_IRQHandler;/* Set Interrupt Vector        */
	  VICVectCntl26  = 15;                          /* use it for Timer2 Interrupt */
	  VICIntEnable  = (1  << 26);                   /* Enable Timer2 Interrupt     */
	  PINSEL0 |= 0x3 << 12; //;0x000C0000;
	  T2EMR = 0x00000030;  //Toggle MAT1[0],MAT1[1] External Match bit/output
	  PCLKSEL1 = 0x00001000; //Timer 2 Clock = 72Mhz

//	  PCLK_TIMER0 Peripheral clock selection for TIMER0. 00
//
//	  T0EMR = 0x000000F3;  //Toggle MAT1[0],MAT1[1] External Match bit/output
//	  PINSEL3 |= 0x000C0000;





//	memcpy(&SetupImg,&BSR_setup_img_t,sizeof(SetupImg_t));
//	dst = (unsigned long*)&SetupImg;
//	src = (unsigned long*)&BSR_setup_img_t;
//	for(i = 0;i<sizeof(SetupImg_t)/4;i++,dst++,src++)
//			*dst = *src;
//
//	if(crc16((unsigned char*)&SetupImg, sizeof(SetupImg_t)) != 0)
//		set_defaults();


//	main_mci();
//
//	MCI_Read_Block( 0 );
//	while ( MCI_Block_End_Flag == 1 );
//	dst = (unsigned long*)&SetupImg;
//	src = (unsigned long*)ReadBlock;
//	for(i = 0;i<sizeof(SetupImg_t)/4;i++,dst++,src++)
//			*dst = *src;
//	if(crc16((unsigned char*)&SetupImg, sizeof(SetupImg_t)) != 0)
//		set_defaults();




//	for(i = 1;i<5;i++)
//	{
//		SetupImg.Timer[i].start = 12*3600 + (52+i)*60;	
//		SetupImg.Timer[i].end = 12*3600 + (54+i)*60;	
//	}
#if 0
	i = 0;
	while(1)
	{
		RTclock.timer = 1000; //4sec timeout
		while(RTclock.timer);
		relay_status = (1<<i);
		SSP1Send( &relay_status, 1 );
		LED_Off (i);
		if(++i == 8)
			i = 0;
		LED_On (i);

	}
#endif
	 
  lcd_init();
  lcd_clear();
  lcd_print ("   GENERATOR    ");
  set_cursor (0, 1);
  lcd_print ("   -*-   -*-    ");

	LED_Off(0);
	LED_On(2);



	RTclock.timer = 4000; //4sec timeout
	while(RTclock.timer);
  while (1) 
  {                           /* Loop forever                       */
//    AD_value = AD_last;                 /* Read AD_last value                 */
//    if (AD_value != AD_last)            /* Make sure that AD interrupt did    */
//      AD_value = AD_last;               /* not interfere with value reading   */
//    AD_print  = AD_value;               /* Get unscaled value for printout    */
//    AD_value /= 13;                     /* Scale to AD_Value to 0 - 78        */
//    if (AD_old != AD_value)  {          /* If AD value has changed            */
//      AD_old = AD_value;
//      Disp_Bargraph(0, 1, AD_value);    /* Display bargraph according to AD   */
//    }
		
		if (clock_1s) 
		{
			clock_1s = 0;
			set_cursor (0, 0);
			sprintf(str,"%02d.%02d.%04d        ",current_time.RTC_Mday,current_time.RTC_Mon,current_time.RTC_Year);
			lcd_print ((const unsigned char*)str);					 

			RTCGetTime(&current_time);
			set_cursor (0, 1);
			sprintf(str,"%02d:%02d:%02d        ",current_time.RTC_Hour,current_time.RTC_Min,current_time.RTC_Sec);
			lcd_print ((const unsigned char*)str);					 

#if 0  		
			time_s = current_time.RTC_Hour*60*60+ current_time.RTC_Min*60 + current_time.RTC_Sec;
			for (i = 0; i<5 ; i++)
			{
				if((SetupImg.Timer[i].start != 0)&& (SetupImg.Timer[i].end != 0))
				{
					if(SetupImg.Timer[i].start < SetupImg.Timer[i].end)
					{
						if(time_s >= SetupImg.Timer[i].start)
						{
							if(time_s < SetupImg.Timer[i].end)
								LED_On (i);
							else
								LED_Off (i);
						}
					}
				}
			}
#endif
    }
		main_loop();
	}
}

/*
void main_scr(uchar screen)
{
	unsigned int val;

	RTCTime timeval;

	switch(screen)
	{
		case 0:
		default:
			RTCGetTime(&current_time);
			set_cursor (0, 0);
			sprintf(str,"%02d.%02d.%04d   %03d",current_time.RTC_Mday,current_time.RTC_Mon,current_time.RTC_Year,keypad_changecntr);
			lcd_print ((const unsigned char*)str);

			set_cursor (0, 1);
			sprintf(str,"%02d:%02d:%02d     %03d",current_time.RTC_Hour,current_time.RTC_Min,current_time.RTC_Sec,keypad_getcntr);
			lcd_print ((const unsigned char*)str);
			break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
			timeval.RTC_Hour = SetupImg.Timer[screen-1].start/3600;
			val = SetupImg.Timer[screen-1].start%3600 ;
			timeval.RTC_Min = val/60;
//			timeval.RTC_Sec = val%60;

			set_cursor (0, 0);
			sprintf(str,"TMR%d  from %02d:%02d",screen, timeval.RTC_Hour,timeval.RTC_Min);
			lcd_print ((const unsigned char*)str);

			timeval.RTC_Hour = SetupImg.Timer[screen-1].end/3600;
			val = SetupImg.Timer[screen-1].end%3600 ;
			timeval.RTC_Min = val/60;

			val = SetupImg.Timer[screen-1].dst_out;
			set_cursor (0, 1);
			if(val<6)
				sprintf(str,"%s  to %02d:%02d",out_names[val],timeval.RTC_Hour,timeval.RTC_Min);
			else
				sprintf(str,"NONE   End %02d:%02d",timeval.RTC_Hour,timeval.RTC_Min);
			lcd_print ((const unsigned char*)str);
			break;
	}
}
*/

void startOutFrequency()
{
	T2EMR = 0x000000F3;
	T2TCR         = 3;
	T2TCR         = 1;
	LED_On(1);
	LED_Off(2);
	runStatus = 1;
}
void stopOutFrequency()
{
	T2TCR         = 0;
//				IOCLR0 = 0x00000040;
	T2EMR = 0x000000F0;
	LED_On(2);
	LED_Off(1);
	runStatus = 0;
}

void refreshLCD(int mode)
{
	lcd_clear();
	if(mode)
	{
		set_cursor (0, 0);
		if(runStatus)
			sprintf(str,"Running");
		else
			sprintf(str,"Stopped");
//		else
//			sprintf(str,"Paused");
		lcd_print ((const unsigned char*)str);

		set_cursor (10, 0);
		if(mode)
			sprintf(str,"  AUTO");
		else
			sprintf(str,"MANUAL");
		lcd_print ((const unsigned char*)str);

		set_cursor (0, 1);
		if(freq_out)
		{
			if(strlen(name)<16)
				sprintf(str,"Freq: %sHz",name);
			else
				sprintf(str,"Freq: %s","");
		}else
		{
			sprintf(str,"   ---OFF---    ");
		}
		lcd_print ((const unsigned char*)str);
	}
	else
	{
//		set_cursor (0, 0);
//		sprintf(str,"%05d.%02dHz",freq_out/100,freq_out%100);
//		lcd_print ((const unsigned char*)str);
		if(runStatus)
			sprintf(str,"Running");
		else
			sprintf(str,"Stopped");
//		else
//			sprintf(str,"Paused");
		lcd_print ((const unsigned char*)str);

		set_cursor (10, 0);
		if(mode)
			sprintf(str,"  AUTO");
		else
			sprintf(str,"MANUAL");
		lcd_print ((const unsigned char*)str);


		set_cursor (0, 1);
		sprintf(str,"Freq: %05d.%02dHz",freq_out/100,freq_out%100);

//		sprintf(str,"%d(%d) %d",num_tics, timer_cntr, num_tics_correct);
		lcd_print ((const unsigned char*)str);
	}
}


unsigned long print_freq(void)
{

	float f;
	double d;
	if(freq_out != 0)
	{

//		f = (float)freq_out/100.0;
//		num_tics = 36000000.0/f;
//		f = (float)freq_out/100.0;

//		num_tics = 3600000000L/(unsigned long)freq_out;
//		num_tics_correct = (unsigned long)((3600000000%freq_out)*3600000000);
		
		d = 3600000000.0/(double)freq_out;
		num_tics = (unsigned long)d;
		num_tics_correct = (unsigned long)((d-num_tics)*100);
		f = (float)36000000L/(float)num_tics;
		freq_out_real = f * 100;
		cycle_counter = 0;
		num_tics_correct_counter = num_tics_correct;
		T2MR0 = num_tics;
		T2TC = 0;
		T2TCR         = 3;
		T2TCR         = 1;

//		T2MCR         = 3;                           /* Interrupt enable and Reset on MR0  */

	}else
	{
		freq_out_real = 0;
//		T2MCR         = 2;                           /* Interrupt disable and Reset on MR0  */

	}

//		num_tics =  (1/freq_out)/2  / 1/72000000
//	num_tics =  (10l*72000000l)/((unsigned long)freq_out*2L);
	return num_tics;
}




#define STAT_BEFORE_DOT	0
#define STAT_AFTER_DOT	1
#define LEN_BEFORE_DOT	5
#define LEN_AFTER_DOT	2

void main_loop(void)
{
	char freq_string[16] = {0};
	unsigned long oldSeconds;
	unsigned long seconds_past = 0;
	int interval_cntr = 0;
	uint evt;
	uchar digit_stat = STAT_BEFORE_DOT;
	uchar digit_position = 0;
	uchar screen = 0;	
	uchar status = 0;
	unsigned long hold_time = clock();
//	unsigned long num_tics = COUNTER100Hz;
	float f,val;
	uchar len;
	lcd_clear();
	beep(500,1);
//	main_scr(screen);
	RTclock.timer = 1000; //4sec timeout
	print_freq();
	refreshLCD(runMode);
	while(1)
	{
		evt = get_evnt_queue();
		switch(evt)
		{
		case WE_A:
			status = 1;
			digit_stat = STAT_BEFORE_DOT;
			freq_out = 0;
			freq_string[0] = 0x00;
			stopOutFrequency();
			break;
		case WE_UPHLD:
		case WE_UPKEY: /*Start button*/
//		case WE_B:
			if(runStatus == 0) /*Start*/
			{
				oldSeconds = RTclock.seconds;
//				interval_cntr = 0;
				if(interval_cntr >= sizeof(struct_freq_table)/sizeof(struct_freq_table[0]))
					interval_cntr = 0;
				sprintf(name,"%s",struct_freq_table[interval_cntr].name);
				freq_out = struct_freq_table[interval_cntr++].real_freq;
				status = 1;
				startOutFrequency();
			}else /*Stop/Pause*/
			{
				seconds_past = RTclock.seconds - oldSeconds;
				if(interval_cntr > 0)
					interval_cntr--;
				status = 1;
				digit_stat = STAT_BEFORE_DOT;
				freq_out = 0;
				freq_string[0] = 0x00;
				stopOutFrequency();
			}
			break;
//		case WE_C:
		case WE_DWNHLD:
		case WE_DWNKEY: /*Stop button*/
			status = 1;
			digit_stat = STAT_BEFORE_DOT;
			freq_out = 0;
			freq_string[0] = 0x00;
			stopOutFrequency();
			break;
		case WE_D:		/*Next Frequency*/
			interval_cntr++;
			if(interval_cntr >= sizeof(struct_freq_table)/sizeof(struct_freq_table[0]))
				interval_cntr = 0;
			sprintf(name,"%s",struct_freq_table[interval_cntr].name);
			freq_out = struct_freq_table[interval_cntr].real_freq;
			status = 1;

			break;
		case WE_E:/*    STAR+          *//*Prev Frequency*/
			if(interval_cntr>0)
				interval_cntr--;
			else
				interval_cntr = sizeof(struct_freq_table)/sizeof(struct_freq_table[0])-1;
			sprintf(name,"%s",struct_freq_table[interval_cntr].name);
			freq_out = struct_freq_table[interval_cntr].real_freq;
			status = 1;
			break;
#if 1
		case WE_DOT:
			if (digit_stat == STAT_AFTER_DOT)
				break;
			len = strlen(freq_string);
			if(len<=LEN_BEFORE_DOT)
			{
				sprintf(freq_string+len,"%c",'.');
				val = atof(freq_string);
				freq_out = (uint)((val*100.0) + 0.5);
				digit_stat = STAT_AFTER_DOT;
				status = 1;
			}
			break;

		case WE_0:
				len = strlen(freq_string);
				if(len<((digit_stat == STAT_AFTER_DOT)?(LEN_BEFORE_DOT+1+LEN_AFTER_DOT):LEN_BEFORE_DOT))
				{
					sprintf(freq_string+len,"%c",'0');
					val = atof(freq_string);
					freq_out = (uint)((val*100.0) + 0.5);
					status = 1;
				}
				break;
			case WE_1:
			case WE_2:
			case WE_3:
			case WE_4:
			case WE_5:
			case WE_6:
			case WE_7:
			case WE_8:
			case WE_9:
				if(freq_out == 0)
					startOutFrequency();

				len = strlen(freq_string);
				if(len<((digit_stat == STAT_AFTER_DOT)?(LEN_BEFORE_DOT+1+LEN_AFTER_DOT):LEN_BEFORE_DOT))
				{
					sprintf(freq_string+len,"%c",evt+0x30);
					val = atof(freq_string);
					freq_out = (uint)((val*100.0) + 0.5);
					status = 1;
				}
				break;
#endif
			case WE_ENTER:
				status = 1;
				hold_time = clock();
				RTclock.timer = 1000;
				break;
			case WE_B:
				if(freq_out>1)
				{
					status = 1;
					startOutFrequency();
				}
				hold_time = clock();
				status = 1;


/* Timer2 Enable               */
				//				freq_out++;
//				if(freq_out>5000000)
//					freq_out = 2;
				break;
			case WE_C:
//			case WE_DWNHLD:
//			case WE_DWNKEY:
				hold_time = clock();
				status = 1;
				runMode = 1- runMode;
//				if(freq_out > 2)
//					freq_out--;
//				else
//					freq_out = 5000000;
				stopOutFrequency();
				break;
			default:
				status = 0;
				if(RTclock.timer == 0)
				{
//					status = 1;
					if(screen == 0)
					{
						refreshLCD(runMode);
						RTclock.timer = 1000;
					}
					else
					{
						RTclock.timer = 1000;
						if(clock() - hold_time > 60) /*More then 1 minute*/
						{
							beep(1000,1);
							screen = 0;
						}
						else
							beep(500,1);
					}
				}

				if((runMode)&&(runStatus)&&((RTclock.seconds - oldSeconds)+seconds_past)>60) /*Every minute*/
				{
					oldSeconds = RTclock.seconds;
					seconds_past = 0;

					if(interval_cntr >= sizeof(struct_freq_table)/sizeof(struct_freq_table[0]))
					{
						interval_cntr = 0;
						digit_stat = STAT_BEFORE_DOT;
						sprintf(name,"---");
						freq_out = 0;
						freq_string[0] = 0x00;
						stopOutFrequency();
					}
					else
					{
						sprintf(name,"%s",struct_freq_table[interval_cntr].name);
						freq_out = struct_freq_table[interval_cntr++].real_freq;
					}
					status = 1;
				}
				break;
		}
		if (status == 0)
			continue;

		print_freq();
		refreshLCD(runMode);
	}
	return;
}



void set_defaults(void)
{
unsigned long* src;
unsigned long* dst;
int i;
		/* write defaults */
		for(i = 0;i<10;i++)
		{
			SetupImg.Timer[i].start = 12*3600;	
			SetupImg.Timer[i].end = 12*3600;	
			SetupImg.Timer[i].dst_out = 0xff;
		}
		/* put checksum to the last halfword */
		((unsigned short*)&SetupImg)[sizeof(SetupImg_t)/2 - 1] = crc16((unsigned char*)&SetupImg, sizeof(SetupImg_t) - 2);
		src = (unsigned long*)&SetupImg;
		dst = (unsigned long*)WriteBlock;
		memcpy((char*)dst,(char*)src,sizeof(SetupImg_t));
//		dst = (unsigned long*)&BSR_setup_img_t;
//		for(i = 0;i<sizeof(SetupImg_t)/4;i++,dst++,src++)
//			*dst = *src;
		MCI_Write_Block(0);
		while ( MCI_Block_End_Flag == 1 );
}

void save_params(void)
{
	unsigned long* src;
	unsigned long* dst;


	((unsigned short*)&SetupImg)[sizeof(SetupImg_t)/2 - 1] = crc16((unsigned char*)&SetupImg, sizeof(SetupImg_t) - 2);
	src = (unsigned long*)&SetupImg;
	dst = (unsigned long*)WriteBlock;
	memcpy((char*)dst,(char*)src,sizeof(SetupImg_t));
//		dst = (unsigned long*)&BSR_setup_img_t;
//		for(i = 0;i<sizeof(SetupImg_t)/4;i++,dst++,src++)
//			*dst = *src;
	MCI_Write_Block(0);
	while ( MCI_Block_End_Flag == 1 );



}

/* Timer2 IRQ: Executed periodically                                          */
__irq void T2_IRQHandler(void)
{
	timer_cntr = T2TC;
	if(num_tics_correct_counter > 0)
	{
		T2MR0 = num_tics + 1;
		num_tics_correct_counter--;
	}
	else
	{
		T2MR0 = num_tics;
	}
	cycle_counter++;
	if(cycle_counter == 100)																														
	{
		cycle_counter = 0;
		num_tics_correct_counter = num_tics_correct;
	}

	T2IR = 1; /* Clear interrupt flag               */
	VICVectAddr = 0; /* Acknowledge Interrupt              */
}

