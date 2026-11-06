/* { dg-do run { target aarch64_sve512_hw } } */
/* { dg-require-effective-target mmap } */
/* { dg-require-effective-target lp64 } */
/* { dg-options "-O3 -fno-strict-overflow -fno-vect-cost-model -msve-vector-bits=512 -fdump-tree-vect-details" } */

/* Two references with equal constant steps, where the lanes of a vector
   iteration that don't run reach past address 0.  The segment that covers those
   lanes wraps around, and the test that compares the end of one segment with
   the start of the other doesn't allow for that.  It misses the overlap between
   the lanes that do run.  */

#include <sys/mman.h>

#define K 0x1000L
#define S (K * 4)

/* A starts below address 0, in the lanes that don't run, and the only lane of
   it that runs lands above address 0.  */

__attribute__ ((noipa)) int
masked (int *a, int *b, short *c, unsigned long n)
{
  int s = 0;
  for (unsigned long i = 0; i < n; i++)
    {
      if (c[i])
	a[i * K] = 42;
      s += b[i * K];
    }
  return s;
}

/* Nothing is masked here, but the loop is fully masked and N is below the
   vectorization factor, so the lanes past N don't run.  */

__attribute__ ((noipa)) int
partial (int *a, int *b, unsigned long n)
{
  int s = 0;
  for (unsigned long i = 0; i < n; i++)
    {
      a[-(long) i * K] = 42;
      s += b[-(long) i * K];
    }
  return s;
}

short c[16];

int
main (void)
{
  char *p = mmap ((void *) (4 * S), 16 * S, PROT_READ | PROT_WRITE,
		  MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
  if (p == MAP_FAILED)
    return 0;

  /* The load reads P at I = 0, before the store writes it at I = 5.  */
  c[5] = 1;
  if (masked ((int *) (p - 5 * S), (int *) p, c, 8) != 0)
    __builtin_abort ();
  if (*(int *) p != 42)
    __builtin_abort ();
  *(int *) p = 0;

  /* The load reads P + S at I = 0, before the store writes it at I = 1.  */
  if (partial ((int *) (p + 2 * S), (int *) (p + S), 2) != 0)
    __builtin_abort ();

  return 0;
}

/* { dg-final { scan-tree-dump-times "LOOP VECTORIZED" 2 "vect" } } */
