
/*****************************************************************************  
 *   rtc.c:  Realtime clock C file for NXP LPC23xx/34xx Family Microprocessors  
 *  
 *   Copyright(C) 2006, NXP Semiconductor  
 *   All rights reserved.  
 *  
 *   History  
 *   2006.07.12  ver 1.00    Prelimnary version, first Release  
 *  
*****************************************************************************/   
#define RTC_PUB

#include <LPC23xx.H>                     /* LPC23xx definitions               */
#include "include.h"

#include "LPC23xx.H"                        /* LPC23xx/24xx definitions */   
//#include "type.h"   
//#include "irq.h"   
#include "rtc.h"   
   
volatile DWORD alarm_on = 0;   


void rtc_init(void)	
{
//RTC_CISS = 0x00;
RTC_CIIR = 0x00;	//disable seconds counter interrupt
RTC_CCR = 0x11; // Enable clock, clock from 32 kHz oscillator
RTC_ILR	 = 0x00;	//disable clock interrupt and inactive alarm interrupt
//RTC_PREINT = 0x0000016D; //set RTC prescalar for 12Mhz;=(PCLK/3276-1
//RTC_PREFRAC= 0x00001B00; // = PCLK - ((PREINT + 1) x 3276

//VICVectAddr13=(unsigned long)RTC_ISR;
//VICVectCntl13=0x0000002D;
//VICIntEnable= 0x00002000;
}


void Init_RTC(void) {
/*RTC Clock Intialization*/
//PCONP |= 0x00000200; /* RTC power enabled*/
RTC_PREINT = 0x0000016E; /* for Xtal @12MHZ */
RTC_PREFRAC = 0x0000001E;
RTC_CCR = 0x00000011;
RTC_ILR =0x1; /*clears the counter increment interrupt*/
RTC_HOUR=12;
RTC_MIN=45;
RTC_SEC=59;
RTC_DOM=21;
RTC_MONTH=12;
RTC_YEAR=2011;

}






   
/*****************************************************************************  
** Function name:       RTCHandler  
**  
** Descriptions:        RTC interrupt handler, it executes based on the  
**              the alarm setting  
**  
** parameters:          None  
** Returned value:      None  
**   
*****************************************************************************/   
void RTCHandler (void) __irq    
{     
  RTC_ILR |= ILR_RTCCIF;        /* clear interrupt flag */   
//  IENABLE;          /* handles nested interrupt */   
   
  alarm_on = 1;   
   
//  IDISABLE;   
  VICVectAddr = 0;      /* Acknowledge Interrupt */   
}   
   
/*****************************************************************************  
** Function name:       RTCInit  
**  
** Descriptions:        Initialize RTC timer  
**  
** parameters:          None  
** Returned value:      None  
**   
*****************************************************************************/   
void RTCInit( void )   
{   
  alarm_on = 0;   
   
  /*--- Initialize registers ---*/       
  RTC_AMR = 0;   
  RTC_CIIR = 0;   
  RTC_CCR = 0;   
  RTC_PREINT = PREINT_RTC;   
  RTC_PREFRAC = PREFRAC_RTC;   
  return;   
}   
   
/*****************************************************************************  
** Function name:       RTCStart  
**  
** Descriptions:        Start RTC timer  
**  
** parameters:          None  
** Returned value:      None  
**   
*****************************************************************************/   
void RTCStart( void )    
{   
  /*--- Start RTC counters ---*/   
  RTC_CCR |= CCR_CLKEN;   
  RTC_ILR = ILR_RTCCIF;   
  return;   
}   
   
/*****************************************************************************  
** Function name:       RTCStop  
**  
** Descriptions:        Stop RTC timer  
**  
** parameters:          None  
** Returned value:      None  
**   
*****************************************************************************/   
void RTCStop( void )   
{      
  /*--- Stop RTC counters ---*/   
  RTC_CCR &= ~CCR_CLKEN;   
  return;   
}    
   
/*****************************************************************************  
** Function name:       RTC_CTCReset  
**  
** Descriptions:        Reset RTC clock tick counter  
**  
** parameters:          None  
** Returned value:      None  
**   
*****************************************************************************/   
void RTC_CTCReset( void )   
{      
  /*--- Reset CTC ---*/   
  RTC_CCR |= CCR_CTCRST;   
  return;   
}   
   
/*****************************************************************************  
** Function name:       RTCSetTime  
**  
** Descriptions:        Setup RTC timer value  
**  
** parameters:          None  
** Returned value:      None  
**   
*****************************************************************************/   
void RTCSetTime( RTCTime Time )    
{   
  RTC_SEC = Time.RTC_Sec;   
  RTC_MIN = Time.RTC_Min;   
  RTC_HOUR = Time.RTC_Hour;   
  RTC_DOM = Time.RTC_Mday;   
  RTC_DOW = Time.RTC_Wday;   
  RTC_DOY = Time.RTC_Yday;   
  RTC_MONTH = Time.RTC_Mon;   
  RTC_YEAR = Time.RTC_Year;       
  return;   
}   
   
/*****************************************************************************  
** Function name:       RTCSetAlarm  
**  
** Descriptions:        Initialize RTC timer  
**  
** parameters:          None  
** Returned value:      None  
**   
*****************************************************************************/   
void RTCSetAlarm( RTCTime Alarm )    
{      
  RTC_ALSEC = Alarm.RTC_Sec;   
  RTC_ALMIN = Alarm.RTC_Min;   
  RTC_ALHOUR = Alarm.RTC_Hour;   
  RTC_ALDOM = Alarm.RTC_Mday;   
  RTC_ALDOW = Alarm.RTC_Wday;   
  RTC_ALDOY = Alarm.RTC_Yday;   
  RTC_ALMON = Alarm.RTC_Mon;   
  RTC_ALYEAR = Alarm.RTC_Year;       
  return;   
}   
   
/*****************************************************************************  
** Function name:       RTCGetTime  
**  
** Descriptions:        Get RTC timer value  
**  
** parameters:          None  
** Returned value:      The data structure of the RTC time table  
**   
*****************************************************************************/   
void RTCGetTime( RTCTime* Time)
{   
       
  Time->RTC_Sec = RTC_SEC;
  Time->RTC_Min = RTC_MIN;
  Time->RTC_Hour = RTC_HOUR;
  Time->RTC_Mday = RTC_DOM;
  Time->RTC_Wday = RTC_DOW;
  Time->RTC_Yday = RTC_DOY;
  Time->RTC_Mon = RTC_MONTH;
  Time->RTC_Year = RTC_YEAR;
  return;
}   
   
/*****************************************************************************  
** Function name:       RTCSetAlarmMask  
**  
** Descriptions:        Set RTC timer alarm mask  
**  
** parameters:          Alarm mask setting  
** Returned value:      None  
**   
*****************************************************************************/   
void RTCSetAlarmMask( DWORD AlarmMask )    
{   
  /*--- Set alarm mask ---*/       
  RTC_AMR = AlarmMask;   
  return;   
}   
   
/*****************************************************************************  
**                            End Of File  
******************************************************************************/   
   


