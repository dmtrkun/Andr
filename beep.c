#define BEEP_PUB

#include "include.h"
unsigned int ms;

// Define a Buffer in DMA RAM to store duty cycle information
//unsigned int BeepA[25] __attribute__((space(dma)));
const unsigned int wave_sample[25] = {
		0,100,300,500,700,800,
		700,500,300,100,0,
		0,100,300,500,700,800,
		700,500,300,100,0
	};


#if 0
void beep(uint time, uchar cnt)
{
#ifdef NO_BEEP	
	return;
#endif
	if (beep_state == BEEPSTATE_OFF)
		return;
	BEEP_OFF;
	beep_cb.beepcnt = beep_cb.beeptmr = 0;
	beep_cb.beepcnt = (cnt-1)<<1;
	beep_cb.beepdrn = beep_cb.beeptmr = time;

//	beep_cb.beeptmr += time;
	BEEP_ON;
}

void strt_snd_dma(void)
{
uchar i;
	i = hwopt & BUZVOL_MSK;
	memcpy(BeepA,wave_sample[i],25);
// Setup and Enable DMA Channel
	DMA3CONbits.AMODE = 0b00; // Register indirect with post increment
	DMA3CONbits.MODE = 0b00; // Continuous, Ping-Pong mode Disabled
	DMA3CONbits.DIR = 1; // Ram to Peripheral
	DMA3PAD = (int)&OC1RS; // Address of the secondary output compare register
	DMA3REQ = 8; // Select Timer3 interrupt as DMA request source
	DMA3CNT = 24; // Number of words to buffer.
	DMA3STA = __builtin_dmaoffset(&BeepA);
	IFS2bits.DMA3IF = 0; // Clear the DMA interrupt flag
	IEC2bits.DMA3IE = 1; // Enable DMA interrupt
	DMA3CONbits.CHEN = 1; // Enable DMA channel
}
void stop_snd_dma(void)
{
// Setup and Enable DMA Channel
	IEC2bits.DMA3IE = 0; // Disable DMA interrupt
	DMA3CONbits.CHEN = 0; // Disable DMA channel
}
// DMA Interrupt Handler
void __attribute__((interrupt, no_auto_psv)) _DMA3Interrupt(void)
{
	IFS2bits.DMA3IF = 0; // Clear the DMA0 Interrupt Flag
}
#endif
