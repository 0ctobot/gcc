/* { dg-require-effective-target vect_int } */
/* { dg-require-effective-target vect_long_long } */

#include "tree-vect.h"

/* The induction shifts X by 9 bits per scalar iteration.  Vectorizing the
   byte sum gives a vectorization factor of 16 on targets with 128-bit
   vectors, so one vector iteration advances X by 144 bits, which is more than
   its type holds.  The check for that must use the vectorization factor
   rather than the number of lanes in X's own vector mode.  */

unsigned char b[64];

__attribute__ ((noipa)) long long
f (unsigned char *p, long long x, unsigned *sum)
{
  unsigned s = 0;
  for (int i = 0; i < 48; i++)
    {
      s += p[i];
      x >>= 9;
    }
  *sum = s;
  return x;
}

int
main (void)
{
  unsigned s;

  check_vect ();

  for (int i = 0; i < 64; i++)
    {
      b[i] = i + 1;
      __asm__ __volatile__ ("" : : : "memory");
    }

  if (f (b, -1, &s) != -1 || s != 1176)
    abort ();

  return 0;
}
