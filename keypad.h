#ifndef keypad_h
#define keypad_h

#ifdef KEYPAD_PUB
#define KEYPADPUB
#else
#define KEYPADPUB extern
#endif



#define BEEP_ON		/*{(OC1CONbits.OCM = 0b101);strt_snd_dma();} // Start the Output Compare mode*/
#define BEEP_OFF	/*{(OC1CONbits.OCM = 0b000);stop_snd_dma();} // Stop the Output Compare mode*/

#define BEEPSTATE_ON	1 
#define BEEPSTATE_OFF	2
#define REPEAT_OFF		0
#define REPEAT_ON			1
 
/* Keys commands*/
#if 1
#define RELEASEKEY      0x00000000
#define STEPPLUS_CMD    0x00000001
#define STEPMIN_CMD     0x00000002
#define PAD1_CMD		0x00000004
#define PAD2_CMD		0x00000008
#define PAD3_CMD		0x00000010
#define PADA_CMD		0x00000020
#define PAD4_CMD		0x00000040
#define PAD5_CMD		0x00000080
#define PAD6_CMD		0x00000100
#define PADB_CMD		0x00000200
#define PAD7_CMD		0x00000400
#define PAD8_CMD		0x00000800
#define PAD9_CMD		0x00001000
#define PADC_CMD		0x00002000
#define PADE_CMD		0x00004000
#define PAD0_CMD		0x00008000
#define PADF_CMD		0x00010000
#define PADD_CMD		0x00020000


#define ENTER_CMD		0x00000004
#define MODE_CMD		0x00000008
#define RUNHOLD_CMD     0x00000010
#define BOLUS_CMD		0x00000020
#define ONOFF_CMD       0x00000040
#define DOOR_CMD		0x00000080
#define MINPLUS_CMD     (STEPMIN_CMD + STEPPLUS_CMD)

#else

typedef struct tagKEYSBITS {
  unsigned MODE_CMD:1;
  unsigned BOLUS_CMD:1;
  unsigned STEPMIN_CMD:1;
  unsigned STEPPLUS_CMD:1;
  unsigned RUNHOLD_CMD:1;
  unsigned ENTER_CMD:1;
  unsigned RSRV:1;
  unsigned ONOFF_CMD:1;
} KEYSBITS;
#ifdef KEYPAD_PUB
KEYSBITS *keysbits = &keypad_stat;
#else
extern KEYSBITS *keysbits;
#endif
#endif

#define WIN_EVNT_BUFLEN		10

/* globals */
KEYPADPUB int keypad_changecntr;
KEYPADPUB int keypad_getcntr;
KEYPADPUB unsigned char keypad_stat;
KEYPADPUB uchar	win_evnt;
KEYPADPUB uchar	win_evnt_smphr;
KEYPADPUB uint	win_evnt_buf[WIN_EVNT_BUFLEN];
KEYPADPUB uchar	win_evnt_first;
KEYPADPUB uchar	win_evnt_last;
KEYPADPUB uint key_time;
KEYPADPUB uchar beep_state;
KEYPADPUB uchar key_repeat_flg;
KEYPADPUB void ChkKeyPad(void);
KEYPADPUB void UpdateDisTimers(void);
KEYPADPUB void event_man(void);
KEYPADPUB void init_keypad(void);
KEYPADPUB void beep(uint time, uchar cnt);
KEYPADPUB uint get_evnt_queue(void);
KEYPADPUB void put_evnt_queue(uint evnt);
KEYPADPUB uint chk_evnt_queue(void);


#define WE_KEYEVENTMASK    32  /* keys mask */
/*----------- window events -----------------*/
#define WE_NOEVENT        0 /* no event */
/* keystrokes */
#define WE_1	        1  /* 1 key pressed */
#define WE_2	        2
#define WE_3        	3
#define WE_4        	4
#define WE_5	        5
#define WE_6        	6
#define WE_7        	7
#define WE_8        	8
#define WE_9        	9
#define WE_0        	10
#define WE_A        	11
#define WE_B        	12
#define WE_C        	13
#define WE_D        	14
#define WE_E        	15
#define WE_DOT        	16

#define WE_UPKEY        17  /* UP key pressed */
#define WE_DWNKEY       18 /* DOWN key pressed */

#define WE_UPHLD        19  /* UP key held down */
#define WE_DWNHLD        20  /* DOWN key held down */
#define WE_UPDWNREL      	21  /* UP or DOWN keys released */
#define WE_BOLUSHLD			  22
#define WE_MODEREL     	23  /* ONOFF key pressed and held down */
#define WE_UPHLD1        24
#define WE_DWNHLD1        25
#define WE_ENTER        26
#define WE_ENTERREL		27

/* hardware events */
#define WE_AIRDETECT			 32
#define WE_DOOROPEN				 33
#define WE_DOORCLOSE			 34
#define WE_BATEMPTY			   35
#define WE_BATLOW			   	 36
#define WE_DOWNOCC		   	 37
#define WE_UPOCC			   	 38
#define WE_MOTORFAULT	   	 39
#define WE_PARAOCC		   	 40

#endif /* keypad_h */
