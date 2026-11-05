/* { dg-additional-options "-O3" } */

#include "tree-vect.h"

/* ivdep rules out dependences between iterations, but it says nothing about the
   two accesses in one iteration.  Grouped stores are emitted at the last store
   of the group, so the load has to be protected by an alias check whether or
   not strict aliasing lets us compute the dependence distance.  */

struct S { int f[1024]; };

__attribute__((noipa)) void
same_type (struct S *p, struct S *q, int n)
{
#pragma GCC ivdep
  for (int i = 0; i < n; i++)
    {
      q->f[2 * i] = 1;
      int t = p->f[2 * i];
      q->f[2 * i + 1] = t;
    }
}

__attribute__((noipa, optimize ("no-strict-aliasing"))) void
no_strict (struct S *p, struct S *q, int n)
{
#pragma GCC ivdep
  for (int i = 0; i < n; i++)
    {
      q->f[2 * i] = 1;
      int t = p->f[2 * i];
      q->f[2 * i + 1] = t;
    }
}

int
main (void)
{
  static struct S a, b;

  check_vect ();

  same_type (&a, &a, 16);
  for (int i = 0; i < 32; i++)
    if (a.f[i] != 1)
      __builtin_abort ();

  no_strict (&b, &b, 16);
  for (int i = 0; i < 32; i++)
    if (b.f[i] != 1)
      __builtin_abort ();

  return 0;
}
