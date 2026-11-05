/* PR tree-optimization/128351 */
/* { dg-add-options vect_early_break } */
/* { dg-require-effective-target vect_early_break_hw } */
/* { dg-require-effective-target vect_int } */
/* { dg-additional-options "-O3 -fno-ivopts" } */

/* { dg-final { scan-tree-dump "LOOP VECTORIZED" "vect" { target aarch64*-*-* } } } */

#include "tree-vect.h"

/* The count of the bottom exit wraps to 0 when B equals E, while the loop never
   runs far enough to get there since the other exit stops it first.  The bound
   recorded for the loop only reflects that other exit, which says nothing about
   whether the bottom exit's count can wrap.  */

__attribute__ ((noipa)) unsigned int
f (unsigned long b, unsigned long e, int lim)
{
  unsigned int s = 0;
  int k = 0;
  unsigned long i = b;

  do
    {
      s += (unsigned int) i;
      k++;
      if (k > lim)
	break;
      i -= 3;
    }
  while (i != e);
  return s + (unsigned int) k;
}

int
main (void)
{
  check_vect ();

  if (f (1, 1, 1000) != 4293467798u)
    abort ();
  return 0;
}
