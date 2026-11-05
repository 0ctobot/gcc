/* { dg-do compile } */
/* { dg-options "-O3 -fno-strict-overflow -fno-vect-cost-model -msve-vector-bits=512 -fdump-tree-vect-details" } */

/* Only the masks say which lanes really run, so the bases of A and B can be
   outside of any object.  Six strides wrap around to 8 bytes here, so the
   address-based test for two writes with equal steps would take a limit of 8
   bytes and miss that A + 4 * S and B + 2 * S are the same location when
   B = A + 2 * S.  */

#define S 3074457345618258604UL
#define K (S / 4)

void
f (int *a, int *b, short *c, short *d, unsigned long n)
{
  n &= 7;
  for (unsigned long i = 1; i < n; i++)
    {
      if (c[i])
	a[i * K] = 42;
      if (d[i])
	b[i * K] = 43;
    }
}

/* { dg-final { scan-tree-dump "Unsupported alias check on lanes that are too far apart" "vect" } } */
