#define KEYPAD_PUB

#include "include.h"
unsigned int ms;

#if 0
// Define a Buffer in DMA RAM to store duty cycle information
unsigned int BeepA[25] __attribute__((space(dma)));
#define WAVE 	120
//#define WAVE1	95
//#define WAVE2 	80
#define WAVE1	105
#define WAVE2 	95
const unsigned int wave_sample[3][25] =
{
	{
		WAVE2,WAVE2,WAVE2,WAVE2,WAVE2,WAVE2,WAVE2,
		WAVE2,WAVE2,WAVE2,WAVE2,WAVE2,WAVE2,
		0,0,0,0,0,0,
		0,0,0,0,0,0
	},
	{
		WAVE1,WAVE1,WAVE1,WAVE1,WAVE1,WAVE1,WAVE1,
		WAVE1,WAVE1,WAVE1,WAVE1,WAVE1,WAVE1,
		0,0,0,0,0,0,
		0,0,0,0,0,0
	},
	{
		WAVE,WAVE,WAVE,WAVE,WAVE,WAVE,WAVE,
		WAVE,WAVE,WAVE,WAVE,WAVE,WAVE,
		0,0,0,0,0,0,
		0,0,0,0,0,0
	},
};
#endif

static int keyUpdateTimer;

uint key_timer;
//static uint key_time;
static uint keygap_time;
int key_bounce_timer; /* de-bounce timer in clock ticks */
static int key, key_old, door, door_old; /* holds a keystroke */
static uint key_latch; /* new keystroke */

#define KEYLTCH_TIME 10		//200mS
void init_keypad(void)
{
	ms = 0;
	keypad_stat = 0;
	beep_state = BEEPSTATE_ON;
	win_evnt_smphr = FALSE;
	memset(win_evnt_buf, 0, WIN_EVNT_BUFLEN);
	win_evnt_first = win_evnt_last = 0;
	key_repeat_flg = REPEAT_ON;
//	PINSEL0 &= 0xFFFFF0F0;                 /* Disable CAM interface, enable Keys */
	IODIR1 &= 0xF3FFFFFF; /* P1.26..27 defined as Inputs         */
//	IO0MASK = 0xFFFFFFCC;

	/*
	 p2.3,	p2.4,	p2.6,	p2.9,	p2.8,	p2.11 defined as Inputs
	 FIODIR2  &= (~0x00000b58);
	 */
	FIO2DIR &= (~0x00000158); //p2.3,	p2.4,	p2.6,	p2.9,

	/*
	 p2.0,	p2.1,	p2.2,	p2.5,	p2.7 defined as Outputs
	 */
	FIO2DIR |= 0x000000a7;

}

uint get_keypad_in(void)
{
	uint keypad_in = 0;
	uchar i;
	uchar j;
	uchar stop = 0;
	uint val;
	uint cnt;
//	uchar scan_order_out[4] =
//	{ 0, 1, 2, 5 };
//	uchar scan_order_in[4] =
//	{ 3, 4, 6, 9 };
	uchar scan_order_out[4] =
//	{ 5, 1, 0 , 2 };
//	{ 0, 1, 2 , 5 };
	{ 2, 1, 5 , 0 };

	uchar scan_order_in[4] =
	{ 9, 6, 4, 3 };
	/*
	 p2.0,	p2.1,	p2.2,	p2.5,	p2.7 defined as Outputs
	 FIOPIN2  |= 0x000000a7; //out
	 */
#if 1
	for (i = 0; i < 4; i++)
	{
		FIO2SET |= 0x000000a7;

		FIO2CLR |= 1 << scan_order_out[i];
		cnt = 150;
		while (cnt--);
		val = FIO2PIN & 0x00000b58;

		for(j = 0; j<4;j++ )
		{
			if ((val & (1 << scan_order_in[j])) == 0)
			{
				stop = 1;
				break;
			}
		}
		FIO2SET |= 0x000000a7;
		if (stop)
		{

			keypad_in |= 1 << ((j + i*4) + 2);
			break;
		}
	}
#else
	val = FIO2PIN & 0x00000b58;

	for(j = 0; j<4;j++ )
	{
		if ((val & (1 << scan_order_in[j])) == 0)
		{
			keypad_in |= 1 << ((i + j*4) + 2);
			stop = 1;
			break;
		}
	}

	i++;
	if(i > 3)
		i = 0;

	FIO2SET |= 0x000000a7;
	FIO2CLR |= 1 << scan_order_out[i];

#endif






//	keypad_in = ((PORTD & 0x0FE0) >> 5)|((DOOR_PIN) << 7);//0x80;
//	keypad_in = (IOPIN1&0x0C000000)>>26;
	keypad_in |= (~(IOPIN1 >> 26)) & 0x00000003;
	return keypad_in;
}

/*
 * Manages window/keypad events
 */
void event_man(void)
{

	uchar update_trig = 0;

	if (key_bounce_timer > 0) /* A keystroke is still being checked */
		return;
	win_evnt = WE_NOEVENT;

	key = key_latch & 0x0003FFFF; /* Hold old keystroke */
	key_time = key_timer; /* Keep time of key was pressed */
	key_bounce_timer = KEYLTCH_TIME; /* Ready for a new keystroke to be accepted */

	if (key == RELEASEKEY && key_old == RELEASEKEY)
	{
	}
	else /* Check a new keystroke */
	{

		if (key == RELEASEKEY) /* key released */
		{
			switch (key_old)
			{

//				case ENTER_CMD:
//   				if(key_time < 4000)  /*4sec*/
//						win_evnt = WE_NOEVENT;
//					else
//						win_evnt = WE_ENTER;
//						break;
//				case BOLUS_CMD:
//					win_evnt = WE_BOLUSREL;
//					break;
//				case MODE_CMD:
//					win_evnt = WE_MODEREL;
//					break;
//				case ONOFF_CMD:
//					win_evnt = WE_ONOFFREL;
//					break;
			case STEPPLUS_CMD:
			case STEPMIN_CMD:
				win_evnt = WE_UPDWNREL;
				break;
			case ENTER_CMD:
				win_evnt = WE_ENTERREL;
				break;
			default:
				win_evnt = WE_NOEVENT;
			}
		}
		else /* key pressed */
		{
			if (key_old == RELEASEKEY)
			{
				beep(15, 1);
				keygap_time = key_timer; /* hold the gap time between two keystrokes */
				switch (key)
				{

				case PAD1_CMD:
					win_evnt = WE_1;
					break;
				case PAD2_CMD:
					win_evnt = WE_2;
					break;
				case PAD3_CMD:
					win_evnt = WE_3;
					break;
				case PAD4_CMD:
					win_evnt = WE_4;
					break;
				case PAD5_CMD:
					win_evnt = WE_5;
					break;
				case PAD6_CMD:
					win_evnt = WE_6;
					break;
				case PAD7_CMD:
					win_evnt = WE_7;
					break;
				case PAD8_CMD:
					win_evnt = WE_8;
					break;
				case PAD9_CMD:
					win_evnt = WE_9;
					break;
				case PAD0_CMD:
					win_evnt = WE_0;
					break;
				case PADA_CMD:
					win_evnt = WE_A;
					break;
				case PADB_CMD:
					win_evnt = WE_B;
					break;
				case PADC_CMD:
					win_evnt = WE_C;
					break;
				case PADD_CMD:
					win_evnt = WE_D;
					break;
				case PADE_CMD:
					win_evnt = WE_E;
					break;
				case PADF_CMD:
					win_evnt = WE_DOT;
					break;
				case STEPPLUS_CMD:
					win_evnt = WE_UPKEY;
					break;
				case STEPMIN_CMD:
					win_evnt = WE_DWNKEY;
					break;
//				case MODE_CMD:
//    	    	  		win_evnt = WE_MODE;
//						break;
//     			case BOLUS_CMD:
//          				win_evnt = WE_BOLUS;
//						break;
//     			case RUNHOLD_CMD:
//          				win_evnt = WE_RUNHOLD;
//						break;
//     			case ENTER_CMD:
//								keypad_getcntr++;
// 		         		win_evnt = WE_ENTER;
//						break;
//     			case ONOFF_CMD:
//          				win_evnt = WE_ONOFF;
//						break;
//     			case MINPLUS_CMD:
//          				win_evnt = WE_MINPLUS;
//						break;
				default:
					win_evnt = WE_NOEVENT;
				}

			}
			else if (key_old == key)
			{
				if (key_time > 1000) /*1sec*/
				{
					if (keyUpdateTimer > 0 || key_repeat_flg == REPEAT_OFF)
						return; /* do not accept a key until interval expires */
					if (key == STEPPLUS_CMD || key == STEPMIN_CMD)
					{
						beep(15, 1);
						if (key_time > 8000) /*10sec*/
							keyUpdateTimer = 50; /* run key update timer 100ms*/
						else if (key_time > 5400) /*5sec*/
							keyUpdateTimer = 100; /* run key update timer 100ms*/
						else if (key_time > 3000) /*3sec*/
							keyUpdateTimer = 300; /* run key update timer 300ms*/
						else
							//if(key_time > 1000)  /*1sec*/
							keyUpdateTimer = 500; /* run key update timer 0.5s*/
					}
					else
					{
						if (key_time > 1000) /*1sec*/
							keyUpdateTimer = 1000; /* run key update timer 1000ms*/
					}
					switch (key)
					{
					case STEPPLUS_CMD:
						if (update_trig)
							win_evnt = WE_UPHLD1;
						else
							win_evnt = WE_UPHLD;
						break;
					case STEPMIN_CMD:
						if (update_trig)
							win_evnt = WE_DWNHLD1;
						else
							win_evnt = WE_DWNHLD;
						break;
//					case ONOFF_CMD:
//						win_evnt = WE_ONOFFHLD;
//						break;
					default:
						win_evnt = WE_NOEVENT;
					}
				}
			}
		}

		/* Accept a key */
		key_old = key; /* Hold old keystroke */
		if (win_evnt != WE_NOEVENT)
		{
			put_evnt_queue(win_evnt);
		}
//		else if (win_evnt == WE_ONOFF)
//		{
//			put_evnt_queue(win_evnt);
//		}
	}
}

///////////////////////////////////////////////////////////////////////////
// Updates display timers
void UpdateDisTimers(void)
{
	static unsigned long lastClock;
	uint elapsedTime;

	elapsedTime = clock_ms() - lastClock; /* time elapsed */
	lastClock = clock_ms();

	if (keyUpdateTimer > 0) /* key update timer */
		keyUpdateTimer -= elapsedTime;
}

///////////////////////////////////////////////////////////////////////////
// Checks keypad status and counts a time while the key is pressed
void ChkKeyPad(void)
{
	static unsigned long lastClock;
	unsigned int elapsedTime;
	uint key_new;

	elapsedTime = clock_ms() - lastClock; /* Time elapsed */
	lastClock = clock_ms();
	key_timer += elapsedTime; /* Count time while a key is being pressed */

	key_new = get_keypad_in(); /* Read a keypad status */
	if (key_bounce_timer > 0) /* Test a key */
	{
		if (key_latch != key_new) /* Key was changed */
		{
			keypad_changecntr++;
			key_latch = key_new; /* Keep new key */
			key_bounce_timer = KEYLTCH_TIME; /* Run de-bounce timer for new keystroke */
			if (key_latch != 0) /* Some key is pressed */
				key_timer = 0; /* Restart key pressed timer */
		}
		else
		{
			key_bounce_timer -= elapsedTime;
//      		if(key_bounce_timer == 0)
//						keypad_changecntr++;
		}
	}
}

void put_evnt_queue(uint evnt)
//void put_evnt_queue(uchar win_event,uchar hw_event)
{
	/*lease semaphore*/ //Interrupt proc ???
	if (win_evnt_smphr == TRUE)
		return;
	win_evnt_smphr = TRUE;

	win_evnt_buf[win_evnt_last] = evnt;
	win_evnt_last++;
	if (win_evnt_last >= WIN_EVNT_BUFLEN)
		win_evnt_last = 0;
	if (win_evnt_last == win_evnt_first)
	{
		win_evnt_first++;
		if (win_evnt_first >= WIN_EVNT_BUFLEN)
			win_evnt_first = 0;
	}
	/*release semaphore*/
	win_evnt_smphr = FALSE;
}

uint get_evnt_queue(void)
{
	uint evnt = 0, i;
	/*enter critical section*/																														 
	GLOB_CRITSECT_START;
	/*lease semaphore*/
	if (win_evnt_smphr == TRUE)
		return 0;
	win_evnt_smphr = TRUE;

	if (win_evnt_first != win_evnt_last)
	{
		evnt = win_evnt_buf[win_evnt_first];
		win_evnt_first++;
		if (win_evnt_first >= WIN_EVNT_BUFLEN)
			win_evnt_first = 0;
	}

	/*release semaphore*/
	win_evnt_smphr = FALSE;
	/*exit critical section*/
	GLOB_CRITSECT_STOP;
//	SSP1Send( &relay_status, 1 );
//	for(i = 0;i<6;i++)
//	{
//		if(relay_status&(1<<i))
//			LED_On(i);
//		else
//			LED_Off(i);
//	}
	return evnt;
}

uint chk_evnt_queue(void)
{
	uint evnt = 0;
	/*enter critical section*/
	GLOB_CRITSECT_START;
	/*lease semaphore*/
	if (win_evnt_smphr == TRUE)
		return 0;
	win_evnt_smphr = TRUE;

	if (win_evnt_first != win_evnt_last)
		evnt = win_evnt_buf[win_evnt_first];

	/*release semaphore*/
	win_evnt_smphr = FALSE;
	/*exit critical section*/
	GLOB_CRITSECT_STOP;
	return evnt;
}

void beep(uint time, uchar cnt)
{
#ifdef NO_BEEP	
	return;
#endif
	if (beep_state == BEEPSTATE_OFF)
		return; BEEP_OFF;
	beep_cb.beepcnt = beep_cb.beeptmr = 0;
	beep_cb.beepcnt = (cnt - 1) << 1;
	beep_cb.beepdrn = beep_cb.beeptmr = time;

	BEEP_ON;
}
