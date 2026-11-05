/* { dg-do compile } */
/* { dg-options "-O3 -fno-strict-overflow -fno-vect-cost-model -msve-vector-bits=512 -fdump-tree-vect-details" } */

/* Only the masks say which lanes really run, so K can be large enough for the
   lanes of one vector to wrap around the address space, and the address-based
   test for two writes with equal steps doesn't allow for that.  K isn't known
   at compile time, so don't use that test on these references at all.  */

void
f (int *a, int *b, short *c, short *d, unsigned long k, unsigned long n)
{
  n &= 7;
  for (unsigned long i = 1; i < n; i++)
    {
      if (c[i])
	a[i * k] = 42;
      if (d[i])
	b[i * k] = 43;
    }
}

/* { dg-final { scan-tree-dump "Unsupported alias check on lanes that are too far apart" "vect" } } */
