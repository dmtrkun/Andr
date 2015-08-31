#ifndef beep_h
#define beep_h

#ifdef BEEP_PUB
#define BEEPPUB
#else
#define BEEPPUB extern
#endif



#define BEEP_ON		/*{(OC1CONbits.OCM = 0b101);strt_snd_dma();} // Start the Output Compare mode*/
#define BEEP_OFF	/*{(OC1CONbits.OCM = 0b000);stop_snd_dma();} // Stop the Output Compare mode*/

#define BEEPSTATE_ON	1 
#define BEEPSTATE_OFF	2
#define REPEAT_OFF		0
#define REPEAT_ON			1
 
/* globals */
BEEPPUB uchar beep_state;
BEEPPUB void beep(uint time, uchar cnt);

#endif /* beep_h */
