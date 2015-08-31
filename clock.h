#ifndef clock_h
#define clock_h

#ifdef CLOCK_PUB
#define CLOCKPUB
#else
#define CLOCKPUB extern
#endif



typedef struct
{
   unsigned long ticks;     /* absolute time, milliseconds   */
   unsigned long seconds;   /* absolute time, seconds        */
   unsigned int timer;     /* countdown timer, milliseconds */
   unsigned int timer2;     /* countdown timer, milliseconds rxuart*/
}clockType;
   
typedef struct
{
	 uchar beepcnt;             /* number od beeps, times */
	 unsigned int beepdrn;     /* beep duration, milliseconds */
	 unsigned int beeptmr;     /* countdown timer, milliseconds */
}beepType;
CLOCKPUB volatile beepType beep_cb;

CLOCKPUB volatile clockType RTclock;

CLOCKPUB short AD_last;
CLOCKPUB unsigned char clock_1s;

CLOCKPUB void init_clock(void);
CLOCKPUB unsigned long clock(void);
CLOCKPUB unsigned long clock_ms(void);

/* Import external IRQ handlers from IRQ.c file                               */
CLOCKPUB __irq void T0_IRQHandler  (void);
CLOCKPUB __irq void ADC_IRQHandler (void);

#endif /* clock_h */
