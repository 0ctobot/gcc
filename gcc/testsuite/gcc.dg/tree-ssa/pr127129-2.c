/* { dg-do run { target lp64 } } */
/* { dg-options "-O2 -fno-tree-vectorize -ftree-loop-distribution" } */

/* The store to A only runs when I is 100, so the distance that its stride
   would travel over the whole loop has nothing to do with any real access.
   That distance is larger than the address space here, and the run-time alias
   test must not conclude from it that the store can be moved away from D.
   The base address of the store is an integer, since a pointer that far
   outside of M would be undefined.  */

#define K (1UL << 54)

__attribute__ ((noipa)) void
f (__UINTPTR_TYPE__ a, int *b, int *c, int *d, unsigned long n)
{
  for (unsigned long i = 1; i < n; i++)
    {
      if (c[i])
	*(int *) (a + i * K * 4) = 100;
      d[i] = d[i - 1] + b[i];
    }
}

int b[300], c[300], m[300];

int
main (void)
{
  for (int i = 0; i < 300; i++)
    b[i] = 1;
  c[100] = 1;

  f ((__UINTPTR_TYPE__) (m + 5) - 100 * K * 4, b, c, m, 259);
  if (m[5] != 100 || m[6] != 6)
    __builtin_abort ();
  return 0;
}
