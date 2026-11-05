/* { dg-additional-options "-O3" } */

#include "tree-vect.h"

/* P and Q may be the same object, in which case the load in each function reads
   the value stored just before it.  Grouped stores are emitted at the last
   store of the group, so the vector code would run the load first and read
   stale data unless it checks P != Q at runtime.  */

struct S { int f[1024]; };

/* Zero dependence distance.  */

__attribute__((noipa)) void
zero_dist (struct S *p, struct S *q, int n)
{
  for (int i = 0; i < n; i++)
    {
      q->f[2 * i] = 1;
      int t = p->f[2 * i];
      q->f[2 * i + 1] = t;
    }
}

/* Negative dependence distance: the load reads the value stored by the previous
   iteration.  */

__attribute__((noipa)) void
neg_dist (struct S *p, struct S *q, int n)
{
  for (int i = 0; i < n; i++)
    {
      q->f[2 * i + 2] = i + 1;
      int t = p->f[2 * i];
      q->f[2 * i + 3] = t;
    }
}

int
main (void)
{
  static struct S a, b;

  check_vect ();

  zero_dist (&a, &a, 16);
#pragma GCC novector
  for (int i = 0; i < 32; i++)
    if (a.f[i] != 1)
      __builtin_abort ();

  neg_dist (&b, &b, 16);
#pragma GCC novector
  for (int i = 0; i < 16; i++)
    if (b.f[2 * i + 2] != i + 1 || b.f[2 * i + 3] != i)
      __builtin_abort ();

  /* The unaliased case stays correct too.  */
  __builtin_memset (&b, 0, sizeof (b));
  zero_dist (&a, &b, 16);
#pragma GCC novector
  for (int i = 0; i < 16; i++)
    if (b.f[2 * i] != 1 || b.f[2 * i + 1] != 1)
      __builtin_abort ();

  return 0;
}

/* { dg-final { scan-tree-dump-times "optimized: loop vectorized" 2 "vect" { target { vect_int && vect_perm } } } } */
/* { dg-final { scan-tree-dump "checking that \[^\n\]* have different addresses" "vect" { target { vect_int && vect_perm } } } } */
