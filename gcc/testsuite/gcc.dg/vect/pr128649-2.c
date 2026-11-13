/* PR tree-optimization/128649 */
/* Loop interchange would swap the two loops and hide the outer loop from the
   vectorizer.  */
/* { dg-additional-options "-O3 -fno-loop-interchange" } */

#include "tree-vect.h"

#define N 16

int b[N + 2];

/* Outer iterations J and J + 2 store to b[J + 2], so at most two consecutive
   outer iterations may run side by side.  */

void __attribute__((noipa))
bar (int n)
{
  for (int j = 0; j < N; j++)
    for (int k = 0; k < n; k++)
      b[j + 2 * k] = j;
}

int
main ()
{
  check_vect ();

  bar (2);

#pragma GCC novector
  for (int i = 0; i < N + 2; i++)
    if (b[i] != (i < N ? i : i - 2))
      abort ();

  return 0;
}
