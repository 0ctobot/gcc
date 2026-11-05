/* { dg-additional-options "-O3 -fopenmp-simd" } */

#include "tree-vect.h"

/* omp simd rules out dependences between iterations, but it says nothing about
   the two accesses in one iteration.  They need an alias check if vectorizing
   moves the load before the store.  That check isn't available when optimizing
   for size, so the cold function has to stay scalar.  */

struct S { int f[1024]; };

__attribute__((noipa)) void
hot_zero (struct S *p, struct S *q, int n)
{
#pragma omp simd
  for (int i = 0; i < n; i++)
    {
      q->f[2 * i] = 1;
      int t = p->f[2 * i];
      q->f[2 * i + 1] = t;
    }
}

__attribute__((noipa, cold)) void
cold_zero (struct S *p, struct S *q, int n)
{
#pragma omp simd
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

  hot_zero (&a, &a, 16);
#pragma GCC novector
  for (int i = 0; i < 32; i++)
    if (a.f[i] != 1)
      __builtin_abort ();

  cold_zero (&b, &b, 16);
#pragma GCC novector
  for (int i = 0; i < 32; i++)
    if (b.f[i] != 1)
      __builtin_abort ();

  return 0;
}

/* { dg-final { scan-tree-dump-times "optimized: loop vectorized" 1 "vect" { target { vect_int && vect_perm } } } } */
/* { dg-final { scan-tree-dump "runtime alias check not supported when optimizing for size" "vect" { target { vect_int && vect_perm } } } } */
