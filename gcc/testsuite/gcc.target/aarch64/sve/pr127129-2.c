/* { dg-do compile } */
/* { dg-options "-O3 -fno-strict-overflow -fno-vect-cost-model -msve-vector-bits=512 -fdump-tree-vect-details" } */

/* The store only runs when C says so, and it reaches A[-(1UL << 57)] when I
   is 248, after wrapping around the address space.  The two segments look
   disjoint if the addresses aren't taken modulo the size of the address space,
   so the vectorizer cannot conclude at compile time that they don't alias.  */

#define K (1UL << 54)

int
f (int *a, short *c)
{
  int s = 0;
  for (unsigned long i = 0; i < 251; i++)
    {
      if (c[i])
	a[i * K] = 100;
      s += a[-(1UL << 57)];
    }
  return s;
}

/* { dg-final { scan-tree-dump-not "can tell at compile time" "vect" } } */
