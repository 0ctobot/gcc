/* { dg-require-effective-target int128 } */

#include "tree-vect.h"

/* The group of four stores to A makes the loop an SLP candidate with a
   vector(4) int alongside a vector(1) __int128, which gives it a
   vectorization factor of 1.  The loads through P and Q are hoisted out of
   the vector loop, so the stores to B and C need runtime alias checks against
   them even though the factor is 1.  */

__attribute__((noipa)) void
foo (int *a, __int128 *b, __int128 *c, __int128 *p, __int128 *q,
     unsigned long n)
{
  for (unsigned long i = 0; i < n; i++)
    {
      a[4 * i] = 1;
      a[4 * i + 1] = 2;
      a[4 * i + 2] = 3;
      a[4 * i + 3] = 4;
      c[i] = *p;
      b[i] = *q;
    }
}

int a[32];
__int128 b[8], c[8], q = 77;

int
main (void)
{
  check_vect ();

#pragma GCC novector
  for (int i = 0; i < 8; i++)
    {
      b[i] = 5 + i;
      c[i] = -1;
    }

  foo (a, b, c, &b[0], &q, 8);

  /* The first iteration reads b[0] before writing it and every later
     iteration sees the value of *q stored by the first one.  */
  if (c[0] != 5)
    abort ();
#pragma GCC novector
  for (int i = 1; i < 8; i++)
    if (c[i] != 77)
      abort ();

  return 0;
}
