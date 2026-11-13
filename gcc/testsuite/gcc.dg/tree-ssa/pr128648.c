/* PR tree-optimization/128648 */
/* { dg-do run } */
/* { dg-options "-O2 -floop-interchange -fno-tree-vectorize -fdump-tree-linterchange-details" } */

/* Different iterations of the nest store to the same element, e.g., A[2] is
   written by (j, k) = (0, 1) and later by (2, 0).  Interchanging the loops
   would make the store from J = 0 the last one.  */

#define NJ 4
#define NK 32
#define N (NJ + 2 * NK)

int a[N];

__attribute__((noipa)) void
foo (void)
{
  for (int j = 0; j < NJ; j++)
    for (int k = 0; k < NK; k++)
      a[j + 2 * k] = j;
}

int
main (void)
{
  foo ();
  for (int x = 0; x < N; x++)
    {
      /* The last store to A[X] comes from the largest J that hits it.  */
      int expect = 0;
      for (int j = 0; j < NJ; j++)
	if (x >= j && (x - j) % 2 == 0 && (x - j) / 2 < NK)
	  expect = j;
      if (a[x] != expect)
	__builtin_abort ();
    }
  return 0;
}

/* { dg-final { scan-tree-dump-not "is interchanged" "linterchange" } } */
