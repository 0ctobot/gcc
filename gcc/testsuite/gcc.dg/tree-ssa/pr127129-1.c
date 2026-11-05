/* { dg-do run } */
/* { dg-options "-O2 -fno-tree-vectorize -ftree-loop-distribution" } */

/* The store to A sits in a conditional block before the exit test, so it can
   run in every one of the N - 1 iterations.  Its segment must cover all of them
   for the run-time alias test to keep loop distribution from moving the store
   away from the read of D[0] in the first iteration.  */

__attribute__ ((noipa)) void
f (int *a, int *b, int *c, int *d, unsigned long n)
{
  for (unsigned long i = 1; i < n; i++)
    {
      if (c[i])
	a[i] = 100;
      d[i] = d[i - 1] + b[i];
    }
}

int b[64], c[64], m[200];

int
main (void)
{
  for (int i = 0; i < 64; i++)
    b[i] = 1;
  c[31] = 1;

  int *d = m + 64;

  /* a[31] is d[0], stored only in the last iteration.  */
  f (d - 31, b, c, d, 32);
  if (d[0] != 100 || d[1] != 1 || d[2] != 2)
    __builtin_abort ();
  return 0;
}
