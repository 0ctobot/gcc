/* { dg-do run } */

#include <stdint.h>

#define BITCAST(DST_TYPE, SRC_TYPE, VALUE)	\
  (union {					\
    SRC_TYPE src;				\
    DST_TYPE dst;				\
  }){ VALUE }.dst

typedef uint8_t v2u8 __attribute__((vector_size(2)));

uint16_t g1;
uint32_t g6;
uint16_t g7 = 7252;
uint8_t g10;
uint8_t g14;

__attribute__((noipa)) void
launch (void)
{
  _Bool c4 = g14;
  v2u8 bc6;
  if (g6 != 4)
    goto lbl_sw_def3;
lbl_sw2:
  bc6 = BITCAST (v2u8, uint16_t, g7);
  if ((int8_t) g10 == -8)
    goto lbl_br7;
  while (c4)
    ;
  g10 = (178 & g14) / 22;
  return;
lbl_sw_def3:
  if (4 != g1)
    {
      g14 = g7;
      goto lbl_sw2;
    }
lbl_br7:
  g1 = BITCAST (uint16_t, v2u8, bc6);
  goto lbl_sw_def3;
}

int
main (void)
{
  launch ();
  if (g10 != 0)
    __builtin_abort ();
  return 0;
}
