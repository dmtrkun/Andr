#define MATH_PUB
#include "crc16.h"

#define GENPOLY 0xA001

#ifdef __CRC16_TABLE
extern unsigned short crc16tab[256];

/*
 * Calculates CRC16 using crc16tab table-lookup
 */
unsigned short crc16(unsigned char *src,  unsigned short len )
{
unsigned int accum = 0xFFFF;
unsigned int i;

 	for (i = 0; i < len; i++, src++)
 	{
   	accum ^= *src; // xor low order byte
   	accum = (accum >> 8 ) ^ crc16tab[accum & 0x00ff];
 	}
 	return accum;
}

#else /*!__CRC16_TABLE*/

/*
 * Calculates CRC16 check sum
 */
unsigned int crc16(unsigned char *src, unsigned int len)
{
unsigned int accum = 0xFFFF;
unsigned int i, j;

  for (i = 0; i < len; i++, src++)
  {
    accum ^= *src;  /* xor low order byte */

		for (j = 0; j < 8; j++)
    {
      if ((accum & 0x0001) != 0)
      {
        accum >>= 1;
        accum ^= GENPOLY;
      }
      else
        accum >>= 1;
    }
  }
  return accum;
}
#endif /*!__CRC16_TABLE*/

