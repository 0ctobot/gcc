/* { dg-additional-options "-O3 -fno-version-loops-for-strides -fvect-cost-model=unlimited" } */

#include "tree-vect.h"

/* P and Q have the same access function, so the dependence distance is zero if
   they're the same object.  That only stays true for different iterations if
   the step isn't zero, which isn't known at compile time here.  */

struct S { int f[1024]; };

int o[64];

__attribute__((noipa)) void
foo (struct S *p, struct S *q, int m, int n)
{
  for (int i = 0; i < n; i++)
    {
      q->f[i * m] = i + 1;
      o[i] = p->f[i * m];
    }
}

int
main (void)
{
  static struct S a;

  check_vect ();

  foo (&a, &a, 0, 32);
  for (int i = 0; i < 32; i++)
    if (o[i] != i + 1)
      __builtin_abort ();

  foo (&a, &a, 3, 32);
  for (int i = 0; i < 32; i++)
    if (o[i] != i + 1)
      __builtin_abort ();

  return 0;
}
