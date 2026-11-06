/* { dg-do run { target aarch64_sve512_hw } } */
/* { dg-require-effective-target mmap } */
/* { dg-require-effective-target lp64 } */
/* { dg-options "-O3 -fno-strict-overflow -fno-vect-cost-model -msve-vector-bits=512 -fno-version-loops-for-strides -fdump-tree-vect-details" } */

/* Variable steps, which the compiler can't check against the size of the
   address space.  */

#include <sys/mman.h>

#define S 0x4000L

/* The loop is fully masked and N is below the vectorization factor, so the
   lanes past N don't run and they wrap below address 0.  */

__attribute__ ((noipa)) long
partial (long *a, long *b, long k, unsigned long n)
{
  long s = 0;
  for (unsigned long i = 0; i < n; i++)
    {
      a[(long) i * k] = 42;
      s += b[(long) i * k];
    }
  return s;
}

/* The two stores are 2 elements apart.  A step past that says nothing if it's
   so large that one store's lane at I = 2 meets the other store's lane at I = 0
   modulo the size of the address space, and the masks allow that here since
   only those lanes run.  */

__attribute__ ((noipa)) void
masked (long *a, short *c, short *d, unsigned long n, long k)
{
  for (unsigned long i = 0; i < n; i++)
    {
      if (c[i])
	a[(long) i * k] = 42;
      if (d[i])
	a[(long) i * k + 2] = 43;
    }
}

long m[32];
short c[16], d[16];

int
main (void)
{
  char *p = mmap ((void *) (4 * S), 16 * S, PROT_READ | PROT_WRITE,
		  MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
  if (p == MAP_FAILED)
    return 0;

  /* The load reads P + S at I = 0, before the store writes it at I = 1.  */
  if (partial ((long *) (p + 2 * S), (long *) (p + S), -S / 8, 2) != 0)
    __builtin_abort ();

  /* The second store runs at I = 0 and the first store runs at I = 2.  */
  c[2] = 1;
  d[0] = 1;
  masked (m, c, d, 8, (1L << 60) + 1);
  if (m[2] != 42)
    __builtin_abort ();

  return 0;
}

/* { dg-final { scan-tree-dump-times "LOOP VECTORIZED" 1 "vect" } } */
