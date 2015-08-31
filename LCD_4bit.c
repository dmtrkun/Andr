/******************************************************************************/
/* LCD.c: Functions for 2 line 16 character Text LCD, with 4-bit interface    */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005-2006 Keil Software. All rights reserved.                */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/
#define LCD_PUB

#include <LPC23xx.H>                     /* LPC23xx definitions               */
#include "include.h"
/*********************** Hardware specific configuration **********************/
//#define IF_4BIT



/*------------------------- Speed dependant settings -------------------------*/

/* If processor works on high frequency delay has to be increased, it can be 
   increased by factor 2^N by this constant                                   */
#define DELAY_2N     4

/*------------------------- Text LCD size definitions ------------------------*/

#define LineLen     16                  /* Width (in characters)              */
#define NumLines     2                  /* Hight (in lines)                   */

/*-------------------- LCD interface hardware definitions --------------------*/

/* PINS: 
   - DB4 = P1.20
   - DB5 = P1.21
   - DB6 = P1.22
   - DB7 = P1.23
   - E   = P1.24
   - RW  = P1.15
   - RS  = P1.14                                                              */


#define MCB2300_V1                      /* First version of MCB2300          */

#define PIN_E                 0x01000000
#define PIN_RW                0x00008000
#define PIN_RS                0x00004000
#define PINS_CTRL             0x0100C000
#ifdef IF_4BIT
#define PINS_DATA             0x00F00000
#else
#define PINS_DATA             0x00FF0000
#endif

/* pin E  setting to 0 or 1                                                   */
#define LCD_E(x)              ((x) ? (IOSET1 = PIN_E)  : (IOCLR1 = PIN_E) );

/* pin RW setting to 0 or 1                                                   */
#define LCD_RW(x)             ((x) ? (IOSET1 = PIN_RW) : (IOCLR1 = PIN_RW));

/* pin RS setting to 0 or 1                                                   */
#define LCD_RS(x)             ((x) ? (IOSET1 = PIN_RS) : (IOCLR1 = PIN_RS));

#ifdef IF_4BIT
/* Reading DATA pins                                                          */
#define LCD_DATA_IN           ((IOPIN1 >> 20) & 0xF)

/* Writing value to DATA pins                                                 */
#define LCD_DATA_OUT(x)       IOCLR1 = PINS_DATA; IOSET1 = ((x & 0xF) << 20);
#else

/* Reading DATA pins                                                          */
#define LCD_DATA_IN           ((IOPIN1 >> 16) & 0xFF)

/* Writing value to DATA pins                                                 */
#define LCD_DATA_OUT(x)       IOCLR1 = PINS_DATA; IOSET1 = ((x & 0xFF) << 16);

#endif

/* Setting all pins to output mode                                            */
#define LCD_ALL_DIR_OUT       IODIR1  |=  PINS_CTRL | PINS_DATA;

/* Setting DATA pins to input mode                                            */
#define LCD_DATA_DIR_IN       IODIR1 &= ~PINS_DATA;

/* Setting DATA pins to output mode                                           */
#define LCD_DATA_DIR_OUT      IODIR1 |=  PINS_DATA;

/******************************************************************************/


/* 8 user defined characters to be loaded into CGRAM (used for bargraph)      */
const unsigned char UserFont[8][8] = {
  { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 },
  { 0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10 },
  { 0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18 },
  { 0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C },
  { 0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E },
  { 0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F },
  { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 },
  { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 }
};


/************************ Global function definitions *************************/


/*******************************************************************************
* Delay in while loop cycles                                                   *
*   Parameter:    cnt:    number of while cycles to delay                      *
*   Return:                                                                    *
*******************************************************************************/

static void delay (int cnt)
{
  cnt <<= DELAY_2N;

  while (cnt--);
}


/*******************************************************************************
* Read status of LCD controller                                                *
*   Parameter:    none                                                         *
*   Return:       Status byte contains busy flag and address pointer           *
*******************************************************************************/

static unsigned char lcd_read_status (void)
{
  unsigned char status;

  LCD_DATA_DIR_IN
  LCD_RS(0)
  LCD_RW(1)
  delay(10);
  LCD_E(1)
  delay(10);
#ifdef IF_4BIT
  status  = LCD_DATA_IN << 4;
  LCD_E(0)
  delay(10);
  LCD_E(1)
  delay(10);
  status |= LCD_DATA_IN;
#else
  status = LCD_DATA_IN;
#endif
  LCD_E(0)
  LCD_DATA_DIR_OUT
  return (status);
}


/*******************************************************************************
* Wait until LCD controller busy flag is 0                                     *
*   Parameter:                                                                 *
*   Return:       Status byte of LCD controller (busy + address)               *
*******************************************************************************/

static unsigned char wait_while_busy (void)
{
  unsigned char status;

  do  {
    status = lcd_read_status();
  }  while (status & 0x80);             /* Wait for busy flag                 */

  return (status);
}


/*******************************************************************************
* Write 4-bits to LCD controller                                               *
*   Parameter:    c:      command to be written                                *
*   Return:                                                                    *
*******************************************************************************/

void lcd_write_4bit (unsigned char c)
{
  LCD_RW(0)
  LCD_E(1)
  LCD_DATA_OUT(c&0x0F)
  delay(10);
  LCD_E(0)
  delay(10);
}

/*******************************************************************************
* Write 8-bits to LCD controller                                               *
*   Parameter:    c:      command to be written                                *
*   Return:                                                                    *
*******************************************************************************/

void lcd_write_8bit (unsigned char c)
{
  LCD_RW(0)
  LCD_E(1)
  LCD_DATA_OUT(c&0xFF)
  delay(10);
  LCD_E(0)
  delay(10);
}
void lcd_write (unsigned char c)
{
#ifdef IF_4BIT
	  lcd_write_4bit (c>>4);
	  lcd_write_4bit (c);
#else
	  lcd_write_8bit (c);
#endif
}


/*******************************************************************************
* Write command to LCD controller                                              *
*   Parameter:    c:      command to be written                                *
*   Return:                                                                    *
*******************************************************************************/

void lcd_write_cmd (unsigned char c)
{
  wait_while_busy();

  LCD_RS(0)
  lcd_write(c);
}


/*******************************************************************************
* Write data to LCD controller                                                 *
*   Parameter:    c:      data to be written                                   *
*   Return:                                                                    *
*******************************************************************************/

static void lcd_write_data (unsigned char c)
{
  wait_while_busy();

  LCD_RS(1)
  lcd_write(c);
}


/*******************************************************************************
* Print Character to current cursor position                                   *
*   Parameter:    c:      character to be printed                              *
*   Return:                                                                    *
*******************************************************************************/

void lcd_putchar (char c)
{ 
  lcd_write_data (c);
}


/*******************************************************************************
* Initialize the LCD controller                                                *
*   Parameter:                                                                 *
*   Return:                                                                    *
*******************************************************************************/

void lcd_init (void)
{ 
  int i;
  unsigned char const *p;

  /* Set all pins for LCD as outputs                                          */
  LCD_ALL_DIR_OUT
  lcd_write_8bit (0x38);
  lcd_write_8bit (0x39);
  lcd_write_8bit (0x14);
  lcd_write_8bit (0x70 +12);
  lcd_write_8bit (0x5D);
  lcd_write_8bit (0x6D);
  delay (15000);

  lcd_write_8bit (0x0C);
  lcd_write_cmd (0x01); //  Clear Display
  lcd_write_8bit (0x06);

  lcd_write_cmd(0x40);                  /* Set CGRAM address counter to 0     */
    p = &UserFont[0][0];
    for (i = 0; i < sizeof(UserFont); i++, p++)
      lcd_putchar (*p);

    lcd_write_cmd(0x80);                  /* Set DDRAM address counter to 0     */

  return;





#ifdef IF_4BIT
  /* Select 4-bit interface             */
  LCD_RS(1)
  lcd_write_4bit (0xF);                 /* Select 4-bit interface             */
  LCD_RS(0)
  delay (15000);
  LCD_RS(0)
  lcd_write_4bit (0x3);                 /* Select 4-bit interface             */
  delay (4100);
  lcd_write_4bit (0x3);
  delay (100);
  lcd_write_4bit (0x3);
//  delay (100);
  lcd_write_4bit (0x2);
//  delay (100);
  lcd_write_cmd (0x28);                 /* 2 lines, 5x8 character matrix      */
#else
  LCD_RS(1)
  lcd_write_8bit (0xFF);                 /* Select 4-bit interface             */
  LCD_RS(0)
  delay (15000);
  LCD_RS(0)
  lcd_write_8bit (0x30);                 /* Select 4-bit interface             */
  delay (4100);
  lcd_write_8bit (0x30);
  delay (100);
  lcd_write_8bit (0x30);
//  delay (100);
  lcd_write_cmd (0x38); 	//Function Set (Interface = 8 bits, Set No. of lines and display font)
//  delay (100);
 #endif

  lcd_write_cmd (0x08); //Display OFF
  lcd_write_cmd (0x01); //  Clear Display
  lcd_write_cmd (0x06);                 /* Entry mode: Move right, no shift   */
  lcd_write_cmd (0x0C);                 /* Display ctrl:Disp=ON,Curs/Blnk=OFF */

  /* Load user-specific characters into CGRAM                                 */
//  lcd_write_cmd(0x40);                  /* Set CGRAM address counter to 0     */
//  p = &UserFont[0][0];
//  for (i = 0; i < sizeof(UserFont); i++, p++)
//    lcd_putchar (*p);
//
//  lcd_write_cmd(0x80);                  /* Set DDRAM address counter to 0     */
}



/*******************************************************************************
* Set cursor position on LCD display                                           *
*   Parameter:    column: column position                                      *
*                 line:   line position                                        *
*   Return:                                                                    *
*******************************************************************************/

void set_cursor (unsigned char column, unsigned char line)
{
  unsigned char address;

  address = (line * 0x40) + column;
  address = 0x80 + (address & 0x7F);
  lcd_write_cmd(address);               /* Set DDRAM address counter to 0     */
}

/*******************************************************************************
* Clear the LCD display                                                        *
*   Parameter:                                                                 *
*   Return:                                                                    *
*******************************************************************************/

void lcd_clear (void)
{
  lcd_write_cmd(0x01);                  /* Display clear                      */
  set_cursor (0, 0);
}


/*******************************************************************************
* Print sting to LCD display                                                   *
*   Parameter:    string: pointer to output string                             *
*   Return:                                                                    *
*******************************************************************************/

void lcd_print (unsigned char const *string)
{
  while (*string)  {
    lcd_putchar (*string++);
  }
}

/******************************************************************************/


/* Timer0 IRQ: Executed periodically                                          */
__irq void T1_IRQHandler(void)
{

	T1MR1         = 17999;

	T1IR = 1; /* Clear interrupt flag               */
	VICVectAddr = 0; /* Acknowledge Interrupt              */
}




