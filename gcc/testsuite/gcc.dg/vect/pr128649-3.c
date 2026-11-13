/* PR tree-optimization/128649 */
/* { dg-require-effective-target vect_int } */
/* { dg-require-effective-target vect_hw_misalign } */
/* Loop interchange would swap the two loops and hide the outer loop from the
   vectorizer.  */
/* { dg-additional-options "-O3 -fno-loop-interchange" } */

#include "tree-vect.h"

#define J 16
#define N 40

int a[J + N], b[J + N], ref_a[J + N], ref_b[J + N];

/* Consecutive outer iterations store to the same element in consecutive inner
   iterations, so the later outer iteration still stores last when the outer
   iterations run side by side.  */

void __attribute__((noipa))
foo (int n)
{
  for (int j = 0; j < J; j++)
    for (int k = 0; k < n; k++)
      a[j - k + N] = j;
}

/* The same with the outer loop running backwards through the array.  */

void __attribute__((noipa))
bar (int n)
{
  for (int j = 0; j < J; j++)
    for (int k = 0; k < n; k++)
      b[J - j + k] = j;
}

int
main ()
{
  check_vect ();

  foo (N);
  bar (N);

#pragma GCC novector
  for (int j = 0; j < J; j++)
#pragma GCC novector
    for (int k = 0; k < N; k++)
      {
	ref_a[j - k + N] = j;
	ref_b[J - j + k] = j;
      }

#pragma GCC novector
  for (int i = 0; i < J + N; i++)
    if (a[i] != ref_a[i] || b[i] != ref_b[i])
      abort ();

  return 0;
}

/* { dg-final { scan-tree-dump "OUTER LOOP VECTORIZED" "vect" } } */
/* { dg-final { scan-tree-dump-times "OUTER LOOP VECTORIZED" 2 "vect" { target vect_perm } } } */
