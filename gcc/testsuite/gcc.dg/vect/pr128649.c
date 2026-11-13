/* PR tree-optimization/128649 */
/* Loop interchange would swap the two loops and hide the outer loop from the
   vectorizer.  */
/* { dg-additional-options "-O3 -fno-loop-interchange" } */

#include "tree-vect.h"

#define N 16

int a[N + 1];

/* Outer iteration J stores to a[J + 1] before outer iteration J + 1 does, so
   running the outer iterations side by side would reverse the two stores.  */

void __attribute__((noipa))
foo (int n)
{
  for (int j = 0; j < N; j++)
    for (int k = 0; k < n; k++)
      a[j + k] = j;
}

int
main ()
{
  check_vect ();

  foo (2);

#pragma GCC novector
  for (int i = 0; i < N + 1; i++)
    if (a[i] != (i < N ? i : N - 1))
      abort ();

  return 0;
}

/* { dg-final { scan-tree-dump-not "OUTER LOOP VECTORIZED" "vect" } } */
