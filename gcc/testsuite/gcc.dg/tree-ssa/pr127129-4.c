/* { dg-do compile } */
/* { dg-options "-O2 -fno-tree-vectorize -ftree-loop-distribution -fdump-tree-ldist-details" } */

/* The runtime alias check can't cover a conditional access this large,
   so loop distribution shouldn't version the loop for it.  */

struct big { char x[1 << 20]; };

void
f (struct big *a, struct big *b, int *c, int *d, int *e,
   unsigned long n)
{
  for (unsigned long i = 1; i < n; i++)
    {
      if (c[i])
	a[i] = b[i];
      d[i] = d[i - 1] + e[i];
    }
}

/* { dg-final { scan-tree-dump-not "runtime alias check" "ldist" } } */
