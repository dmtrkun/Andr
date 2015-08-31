#include<stdio.h>
#include<lpc23xx.h>
#if 0
void init(void);
void start(void);
void slave_selectwrt(void);
void word_addr(void);
void slave_selectrd(void);
void data_rd(void);
void stop(void);
void delay(unsigned int cnt);

int main()
  {
  init();
  start();
  slave_selectwrt();
  word_addr();
  start();
  slave_selectrd();
  data_rd();
  stop();
  }

void init(void)
  {
  CCLKCFG = 0x8F;                               //  CCLK = 2MHz
  PCLKSEL0 |= 0x00000100;               //  UART1_pclk=CCLK
  PCLKSEL1 |= 0x00000040;               //      I2C1_pclk = CCLK
  PINSEL0 |= 0x40000000;                /* select pin function fot UART1 TXD*/
  PINSEL0 |= 0x0000000F;                /* select pin function fot I2C1 pin 66 & 67 */
  I21SCLH = 500;                                // make I2C1 bit FREQ = 100
  I21SCLL = 500;

/*-- UART1 configuration start--*/
  U1LCR = 0x83;
  U1DLM = 0;
  U1DLL = 0xD;                                  // baud rate = 9615
  U1LCR = 0x03;
  I21CONCLR = 0x6C;                             // clearing all flags
  I21CONSET = 0x40;                             // enabling I2C
  }

void start(void)
  {
  I21CONSET = 0x20;                             // start condition
  I21CONCLR = 0x08;                             // clear SI
  delay(1000);
  }

void slave_selectwrt(void)
  {
  I21DAT = 0xA0;                                /* EEPROM select + write bit*/
  I21CONCLR = 0x28;                             /* Clear start bit and SI*/
  delay(1000);
  }
void word_addr(void)
  {
  I21DAT = 0x01;
  I21CONCLR = 0x08;                             // clear SI
  delay(1000);
  }
void slave_selectrd(void)
  {
  I21CONCLR = 0x28;                             // Clear start bit and SI
  I21DAT = 0xA1;                                // EEPROM select + Read bit
  I21CONCLR = 0x08;
  delay(1000);
  }
void data_rd(void)
  {
  char k;
  delay(100);
  k = I21DAT;
  U1THR = k;                                    // Send data to Hyper terminal
  I21CONCLR = 0x08;                             // Clear SI
  delay(1000);
  }

void stop(void)
  {
  I21CONSET = 0x10;                             /* Send Stop condition */
  I21CONCLR = 0x08;                             // Clear SI
  delay(1000);
  }
void delay (unsigned int cnt)
  {
  while(cnt--);                                 // delay subroutine
  }
#endif

/*****************************************************************************  
 *   i2c.c:  I2C C file for NXP LPC23xx/24xx Family Microprocessors  
 *  
 *   Copyright(C) 2006, NXP Semiconductor  
 *   All rights reserved.  
 *  
 *   History  
 *   2006.07.19  ver 1.00    Prelimnary version, first Release  
 *  
*****************************************************************************/   
#include "LPC230x.h"                        /* LPC23xx/24xx definitions */   
#include "type.h"   
#include "irq.h"   
#include "i2c.h"   
   
volatile DWORD I2CMasterState = I2C_IDLE;   
volatile DWORD I2CSlaveState = I2C_IDLE;   
   
volatile DWORD I2CCmd;   
volatile DWORD I2CMode;   
   
volatile BYTE I2CMasterBuffer[BUFSIZE];   
volatile BYTE I2CSlaveBuffer[BUFSIZE];   
volatile DWORD I2CCount = 0;   
volatile DWORD I2CReadLength;   
volatile DWORD I2CWriteLength;   
   
volatile DWORD RdIndex = 0;   
volatile DWORD WrIndex = 0;   
   
/*   
From device to device, the I2C communication protocol may vary,   
in the example below, the protocol uses repeated start to read data from or   
write to the device:  
For master read: the sequence is: STA,Addr(W),offset,RE-STA,Addr(r),data...STO   
for master write: the sequence is: STA,Addr(W),length,RE-STA,Addr(w),data...STO  
Thus, in state 8, the address is always WRITE. in state 10, the address could   
be READ or WRITE depending on the I2CCmd.  
*/      
   
/*****************************************************************************  
** Function name:       I2C0MasterHandler  
**  
** Descriptions:        I2C0 interrupt handler, deal with master mode  
**              only.  
**  
** parameters:          None  
** Returned value:      None  
**   
*****************************************************************************/   
void I2C0MasterHandler (void) __irq    
{   
    BYTE StatValue;   
   
    /* this handler deals with master read and master write only */   
    StatValue = I20STAT;   
    IENABLE;                /* handles nested interrupt */     
    switch ( StatValue )   
    {   
    case 0x08:          /* A Start condition is issued. */   
    I20DAT = I2CMasterBuffer[0];   
    I20CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);   
    I2CMasterState = I2C_STARTED;   
    break;   
       
    case 0x10:          /* A repeated started is issued */   
    if ( I2CCmd == LM75_TEMP )   
    {   
        I20DAT = I2CMasterBuffer[2];   
    }   
    I20CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);   
    I2CMasterState = I2C_RESTARTED;   
    break;   
       
    case 0x18:          /* Regardless, it's a ACK */   
    if ( I2CMasterState == I2C_STARTED )   
    {   
        I20DAT = I2CMasterBuffer[1+WrIndex];   
        WrIndex++;   
        I2CMasterState = DATA_ACK;   
    }   
    I20CONCLR = I2CONCLR_SIC;   
    break;   
       
    case 0x28:  /* Data byte has been transmitted, regardless ACK or NACK */   
    case 0x30:   
    if ( WrIndex != I2CWriteLength )   
    {      
        I20DAT = I2CMasterBuffer[1+WrIndex]; /* this should be the last one */   
        WrIndex++;   
        if ( WrIndex != I2CWriteLength )   
        {      
            I2CMasterState = DATA_ACK;   
        }   
        else   
        {   
            I2CMasterState = DATA_NACK;   
            if ( I2CReadLength != 0 )   
            {   
                I20CONSET = I2CONSET_STA;   /* Set Repeated-start flag */   
                I2CMasterState = I2C_REPEATED_START;   
            }   
        }   
    }   
    else   
    {   
        if ( I2CReadLength != 0 )   
        {   
            I20CONSET = I2CONSET_STA;   /* Set Repeated-start flag */   
            I2CMasterState = I2C_REPEATED_START;   
        }   
        else   
        {   
            I2CMasterState = DATA_NACK;   
        }   
    }   
    I20CONCLR = I2CONCLR_SIC;   
    break;   
       
    case 0x40:  /* Master Receive, SLA_R has been sent */   
    I20CONCLR = I2CONCLR_SIC;   
    break;   
       
    case 0x50:  /* Data byte has been received, regardless following ACK or NACK */   
    case 0x58:   
    I2CMasterBuffer[3+RdIndex] = I20DAT;   
    RdIndex++;   
    if ( RdIndex != I2CReadLength )   
    {      
        I2CMasterState = DATA_ACK;   
    }   
    else   
    {   
        RdIndex = 0;   
        I2CMasterState = DATA_NACK;   
    }   
    I20CONSET = I2CONSET_AA;    /* assert ACK after data is received */   
    I20CONCLR = I2CONCLR_SIC;   
    break;   
       
    case 0x20:          /* regardless, it's a NACK */   
    case 0x48:   
    I20CONCLR = I2CONCLR_SIC;   
    I2CMasterState = DATA_NACK;   
    break;   
       
    case 0x38:          /* Arbitration lost, in this example, we don't  
                    deal with multiple master situation */   
    default:   
    I20CONCLR = I2CONCLR_SIC;      
    break;   
    }   
    IDISABLE;   
    VICVectAddr = 0;        /* Acknowledge Interrupt */   
}   
   
/*****************************************************************************  
** Function name:       I2CStart  
**  
** Descriptions:        Create I2C start condition, a timeout  
**              value is set if the I2C never gets started,  
**              and timed out. It's a fatal error.   
**  
** parameters:          None  
** Returned value:      true or false, return false if timed out  
**   
*****************************************************************************/   
DWORD I2CStart( void )   
{   
    DWORD timeout = 0;   
    DWORD returnValue = FALSE;   
    
    /*--- Issue a start condition ---*/   
    I20CONSET = I2CONSET_STA;   /* Set Start flag */   
       
    /*--- Wait until START transmitted ---*/   
    while( 1 )   
    {   
        if ( I2CMasterState == I2C_STARTED )   
        {   
            returnValue = TRUE;   
            break;     
        }   
        if ( timeout >= MAX_TIMEOUT )   
        {   
            returnValue = FALSE;   
            break;   
        }   
        timeout++;   
    }   
    return( returnValue );   
}   
   
/*****************************************************************************  
** Function name:       I2CStop  
**  
** Descriptions:        Set the I2C stop condition, if the routine  
**              never exit, it's a fatal bus error.  
**  
** parameters:          None  
** Returned value:      true or never return  
**   
*****************************************************************************/   
DWORD I2CStop( void )   
{   
    I20CONSET = I2CONSET_STO;      /* Set Stop flag */    
    I20CONCLR = I2CONCLR_SIC;  /* Clear SI flag */    
               
    /*--- Wait for STOP detected ---*/   
    while( I20CONSET & I2CONSET_STO );   
    return TRUE;   
}   
   
/*****************************************************************************  
** Function name:       I2CInit  
**  
** Descriptions:        Initialize I2C controller  
**  
** parameters:          I2c mode is either MASTER or SLAVE  
** Returned value:      true or false, return false if the I2C  
**              interrupt handler was not installed correctly  
**   
*****************************************************************************/   
DWORD I2CInit( DWORD I2cMode )    
{   
    PCONP |= (1 << 19);   
    PINSEL1 &= ~0x03C00000;   
    PINSEL1 |= 0x01400000;  /* set PIO0.27 and PIO0.28 to I2C0 SDA and SCK */   
                            /* function to 01 on both SDA and SCK. */   
    /*--- Clear flags ---*/   
    I20CONCLR = I2CONCLR_AAC | I2CONCLR_SIC | I2CONCLR_STAC | I2CONCLR_I2ENC;       
   
    /*--- Reset registers ---*/   
    I20SCLL   = I2SCLL_SCLL;   
    I20SCLH   = I2SCLH_SCLH;   
    if ( I2cMode == I2CSLAVE )   
    {   
        I20ADR = LM75_ADDR;   
    }       
   
    /* Install interrupt handler */    
    if ( install_irq( I2C0_INT, (void *)I2C0MasterHandler, HIGHEST_PRIORITY ) == FALSE )   
    {   
        return( FALSE );   
    }   
    I20CONSET = I2CONSET_I2EN;   
    return( TRUE );   
}   
   
/*****************************************************************************  
** Function name:       I2CEngine  
**  
** Descriptions:        The routine to complete a I2C transaction  
**              from start to stop. All the intermitten  
**              steps are handled in the interrupt handler.  
**              Before this routine is called, the read  
**              length, write length, I2C master buffer,  
**              and I2C command fields need to be filled.  
**              see i2cmst.c for more details.   
**  
** parameters:          None  
** Returned value:      true or false, return false only if the  
**              start condition can never be generated and  
**              timed out.   
**   
*****************************************************************************/   
DWORD I2CEngine( void )    
{   
    I2CMasterState = I2C_IDLE;   
    RdIndex = 0;   
    WrIndex = 0;   
    if ( I2CStart() != TRUE )   
    {   
        I2CStop();   
        return ( FALSE );   
    }   
   
    while ( 1 )   
    {   
        if ( I2CMasterState == DATA_NACK )   
        {   
            I2CStop();   
            break;   
        }   
    }       
    return ( TRUE );         
}   
   
/******************************************************************************  
**                            End Of File  
******************************************************************************/   
   

 
