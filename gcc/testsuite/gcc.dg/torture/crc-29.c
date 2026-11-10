/* { dg-do compile } */
/* { dg-options "-fdump-tree-crc-details" } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-g" "-Os" "-flto" } } */
/* { dg-require-effective-target int32plus } */

/* This loop computes a CRC of crc alone, since 'j' is initialized with 4
   (100) and increased by 2 each time, so 'j & 1' is always 0.  It mustn't
   be replaced with a CRC that takes 'j' as the data, because the expansion
   would read bit I of j's initial value 4 on iteration I and mix in bit 2.
   'j' isn't shifted by one bit per iteration, so the loop is rejected.  */

#include <stdint.h>

uint16_t crc (uint8_t data, uint16_t crc) {
  uint8_t x16 = 0, carry = 0;
  for (uint8_t i = 0, j=4; i < 8; i++, j+=2) {
      x16 = (uint8_t) (((uint8_t) crc & 1)  ^ (j & 1));
      data >>= 1;
      if (x16 == 1) {
	  crc ^= 0x4002;
	  carry = 1;
	} else
	carry = 0;
      crc >>= 1;
      if (carry)
	crc |= 0x8000;
      else
	crc &= 0x7fff;
    }
  return crc;
}

/* { dg-final { scan-tree-dump "Data isn't shifted by one bit per iteration" "crc" } } */
/* { dg-final { scan-tree-dump-not "calculates CRC!" "crc" } } */
