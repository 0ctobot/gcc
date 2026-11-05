/* { dg-do compile } */
/* { dg-options "-fdump-tree-crc-details" } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-Os" "-flto" } } */
/* { dg-require-effective-target int32plus } */

#include <stdint.h>
uint32_t _crc32 (uint32_t data, uint32_t crc) {
  int i;
  for (i = 0; i < 24; i++) {
      if ((crc & 0x80000000) >> 8 ^ (data & 0x800000))
	crc = (crc << 1) ^ 0x04C11DB7;
      else
	crc = (crc << 1);
      data <<= 1;
    }
  return crc;
}

/* 24-iteration loops aren't treated as CRC loops (PR127984), so the loop is
   seen but must not be recognized.  */
/* { dg-final { scan-tree-dump "Loop iteration number is 23" "crc" } } */
/* { dg-final { scan-tree-dump-not "maybe contains CRC calculation" "crc" } } */
/* { dg-final { scan-tree-dump-not "Bit forward" "crc" } } */
