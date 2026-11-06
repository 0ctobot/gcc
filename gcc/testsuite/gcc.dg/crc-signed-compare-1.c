/* The verification of a CRC loop follows the loop's exit test on a signed
   counter, so a counter going through zero must compare as a signed value.
   Comparing it as unsigned either ended the check early or kept it running for
   about 2^32 iterations, which looked like the compiler hanging.  */
/* { dg-do run } */
/* { dg-require-effective-target int32plus } */
/* { dg-options "-O2 -fno-tree-loop-ivcanon -fno-ivopts -fdump-tree-crc-details" } */
/* { dg-timeout 60 } */

#define LOOP(NAME, ATTR, HEAD)			\
  ATTR unsigned char				\
  NAME (unsigned char crc)			\
  {						\
    for (HEAD)					\
      {						\
	if (crc & 0x80)				\
	  crc = (crc << 1) ^ 7;			\
	else					\
	  crc <<= 1;				\
      }						\
    return crc;					\
  }

#define OPT __attribute__ ((noipa))
#define NOOPT __attribute__ ((noipa, optimize ("O0")))

LOOP (up_through_zero, OPT, int i = -9; i < 6; i += 2)
LOOP (down_to_positive, OPT, int i = 23; i >= 2; i -= 3)
LOOP (up_to_negative, OPT, int i = -23; i <= -2; i += 3)
LOOP (reference, NOOPT, int i = 0; i < 8; i++)

int
main (void)
{
  for (int x = 0; x < 256; x++)
    if (up_through_zero (x) != reference (x)
	|| down_to_positive (x) != reference (x)
	|| up_to_negative (x) != reference (x))
      __builtin_abort ();
  return 0;
}

/* { dg-final { scan-tree-dump-times "calculates CRC!" 3 "crc" } } */
