/* { dg-do run { target lp64 } } */
/* { dg-options "-O2 -fno-tree-vectorize -ftree-loop-distribution -fwrapv-pointer" } */

/* The store to A only runs when I is 130, which is more than PTRDIFF_MAX bytes
   away from where A starts.  A can do that with -fwrapv-pointer.  The full
   distance of 258 strides doesn't fit in the address space, so the run-time
   alias test can't describe A's accesses.  The store must not be moved away
   from D.  */

#define K (1UL << 54)

__attribute__ ((noipa)) void
f (int *a, int *b, int *c, int *d, unsigned long n)
{
  for (unsigned long i = 1; i < n; i++)
    {
      if (c[i])
	a[i * K] = 100;
      d[i] = d[i - 1] + b[i];
    }
}

int b[300], c[300], m[300];

int
main (void)
{
  for (int i = 0; i < 300; i++)
    b[i] = 1;
  c[130] = 1;

  f ((int *) ((char *) (m + 5) - 130 * K * 4), b, c, m, 259);
  if (m[5] != 100 || m[6] != 6)
    __builtin_abort ();
  return 0;
}
