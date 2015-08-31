/******************************************************************************/
/* IRQ.C: IRQ Handler                                                         */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005-2006 Keil Software. All rights reserved.                */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/
#define CLOCK_PUB

#include <LPC23xx.H>                    /* LPC23xx definitions                */
#include "include.h"
#include "rtc.h"                        
#include "main.h"                        

short AD_last; /* Last converted value               */
unsigned char clock_1s; /* Flag activated each second         */


unsigned long clock_ms(void)
{

	return RTclock.ticks;
}
unsigned long clock(void)
{
	return RTclock.seconds;
}

void init_clock(void)
{
	RTclock.timer = 0; /* clear software registers  */
	RTclock.ticks = 0;
	RTclock.seconds = 0;

	beep_cb.beeptmr = 0;
	beep_cb.beepcnt = 0;

}

void beep_hndl(void)
{
	if (beep_cb.beeptmr > 0) /* if countdown timer is active */
		beep_cb.beeptmr -= 1; /*  decrement it               */
	else
	{
		if (beep_cb.beepcnt > 0)
		{
			if (beep_cb.beepcnt & 0x01)
			{
				beep_cb.beeptmr = beep_cb.beepdrn;
				BEEP_ON;
			}
			else
			{
				beep_cb.beeptmr = beep_cb.beepdrn / 10;
				BEEP_OFF;
			}
			beep_cb.beepcnt--;
		}
		else
			BEEP_OFF;
	}
}

/* Timer0 IRQ: Executed periodically                                          */
__irq void T0_IRQHandler(void)
{
	static int clk_cntr;
	unsigned int time_s, i, out;

	if (RTclock.timer > 0) /* if countdown timer is active */
		RTclock.timer -= 1; /*  decrement it               */
	beep_hndl();

	if (RTclock.timer2 > 0) /* if countdown timer is active */
		RTclock.timer2 -= 1; /*  decrement it               */

	RTclock.ticks++; /* increment ticks counter     */
	clk_cntr++;
	if (clk_cntr >= 1000)
	{
		CPU_LED_Toggle();


		clk_cntr = 0;
		clock_1s = 1; /* Activate flag every 1 second       */
		RTclock.seconds++; /*  increment seconds   */
#if 0
		if (test_flg == 0)
		{
			time_s = current_time.RTC_Hour * 60 * 60 + current_time.RTC_Min * 60
					+ current_time.RTC_Sec;
			for (i = 0; i < 10; i++)
			{
				if ((SetupImg.Timer[i].start != 0)
						&& (SetupImg.Timer[i].end != 0))
				{
					out = SetupImg.Timer[i].dst_out;
					if (out == 0xff) //timer is disbled
						continue;

					if (SetupImg.Timer[i].start < SetupImg.Timer[i].end)
					{
						if (time_s >= SetupImg.Timer[i].start)
						{
							if (time_s < SetupImg.Timer[i].end)
								setBit(relay_status, SetupImg.Timer[i].dst_out);
//								LED_On (i);
							else
								clearBit(relay_status,
										SetupImg.Timer[i].dst_out);
//								LED_Off (i);
						}
					}
					else
					{
//					if(time_s >= SetupImg.Timer[i].end)
//					{
//						if(time_s > SetupImg.Timer[i].start)
//							setBit(relay_status,SetupImg.Timer[i].dst_out);
//								LED_On (i);
//						else
//							clearBit(relay_status,SetupImg.Timer[i].dst_out);
//								LED_Off (i);
//					}
					}
				}

			}

			if (test_cntr++ > 0)
			{
				LED_On(1);
				test_cntr = 0;
			}
			else
			{
				LED_Off(1);
			}

		}
		else
		{
			if (test_cntr++ > 0)
			{
				setBit(relay_status, 0);
				test_cntr = 0;
			}
			else
			{
				clearBit(relay_status, 0);
			}
		}
#endif
	}
	/*read keypad*/
	ChkKeyPad();/*every 1 ms*/
	if (RTclock.ticks % 10 == 0) /*every 10 ms*/
	{
		UpdateDisTimers();
	}
	if (RTclock.ticks % 5 == 0) /*every 20 ms*/
	{
		event_man();
	}

	T0IR = 1; /* Clear interrupt flag               */
	VICVectAddr = 0; /* Acknowledge Interrupt              */
}

/* A/D IRQ: Executed when A/D Conversion is done                              */
__irq void ADC_IRQHandler(void)
{

	AD_last = (AD0DR0 >> 6) & 0x3FF; /* Read Conversion Result             */

	VICVectAddr = 0; /* Acknowledge Interrupt              */
}

