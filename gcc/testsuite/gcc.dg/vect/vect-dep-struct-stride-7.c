/* { dg-additional-options "-O3 -fno-strict-aliasing -fno-vect-cost-model" } */

/* A step of K times the size of the structure isn't necessarily a multiple of
   that size, since the unsigned multiplication can wrap.  When 3 * K is 1, a
   step of K * sizeof (struct s) is just sizeof (int), which makes p->y of one
   iteration the same object as p->x of the next one.  */

#include "tree-vect.h"

struct s { int x, y, z; };

#define N 40

int m[200];

__attribute__ ((noipa)) unsigned int
f (char *b, __SIZE_TYPE__ n, __SIZE_TYPE__ k)
{
  unsigned int t = 0;
  for (__SIZE_TYPE__ i = 0; i < n; i++)
    {
      struct s *p = (struct s *) (b + i * k * sizeof (struct s));
      p->x = i + 1;
      t += p->y;
    }
  return t;
}

int
main (void)
{
  __SIZE_TYPE__ k = (__SIZE_TYPE__) -1 / 3 * 2 + 1;
  unsigned int t = 0;

  check_vect ();

  if (k * 3 != 1 || sizeof (struct s) != 3 * sizeof (int))
    return 0;

#pragma GCC novector
  for (int i = 0; i < 200; i++)
    {
      m[i] = 1000 + i;
      if (i > 0 && i <= N)
	t += 1000 + i;
    }

  if (f ((char *) m, N, k) != t)
    __builtin_abort ();
  return 0;
}
