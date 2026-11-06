/* { dg-do run { target { *-*-linux* && aarch64_sve_hw } } } */
/* { dg-require-effective-target mmap } */
/* { dg-require-effective-target lp64 } */
/* { dg-options "-O3 -fno-strict-overflow -fno-vect-cost-model -fdump-tree-vect-details" } */

/* Two masked references that start on opposite sides of address 0 and only run
   once they're both above it, where they overlap.  WHILERW and WHILEWR compare
   the start addresses as plain numbers once before the loop, so they would say
   that the references are far apart.  */

#include <sys/auxv.h>
#include <sys/mman.h>

#define HWCAP2_SVE2_BIT (1UL << 1)

#define I 0x4000UL
#define N (I + 16)
#define ADDR 0x10000UL

#define BELOW ((int *) -8L)
#define ABOVE ((int *) 8L)

__attribute__ ((noipa)) int
raw (int *a, int *b, int *c, int *d, unsigned long n)
{
  int s = 0;
  for (unsigned long i = 0; i < n; i++)
    {
      if (c[i])
	a[i] = 42;
      if (d[i])
	s += b[i];
    }
  return s;
}

__attribute__ ((noipa)) int
war (int *a, int *b, int *c, int *d, unsigned long n)
{
  int s = 0;
  for (unsigned long i = 0; i < n; i++)
    {
      if (d[i])
	s += b[i];
      if (c[i])
	a[i] = 42;
    }
  return s;
}

__attribute__ ((noipa)) void
waw (int *a, int *b, int *c, int *d, unsigned long n)
{
  for (unsigned long i = 0; i < n; i++)
    {
      if (c[i])
	a[i] = 1;
      if (d[i])
	b[i] = 2;
    }
}

int c[N], d[N];

int
main (void)
{
  if (!(getauxval (AT_HWCAP2) & HWCAP2_SVE2_BIT))
    return 0;

  char *p = mmap ((void *) ADDR, 0x1000, PROT_READ | PROT_WRITE,
		  MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
  if (p == MAP_FAILED)
    return 0;
  int *x = (int *) (p + 8);

  /* B reads X at I, before A writes it at I + 4.  */
  c[I + 4] = 1;
  d[I] = 1;
  if (raw (BELOW, ABOVE, c, d, N) != 0)
    __builtin_abort ();
  if (*x != 42)
    __builtin_abort ();
  *x = 0;

  /* A writes X at I, before B reads it at I + 4.  */
  c[I + 4] = 0;
  d[I] = 0;
  c[I] = 1;
  d[I + 4] = 1;
  if (war (ABOVE, BELOW, c, d, N) != 42)
    __builtin_abort ();
  *x = 0;

  /* B writes X at I, before A writes it at I + 4.  */
  c[I] = 0;
  d[I + 4] = 0;
  c[I + 4] = 1;
  d[I] = 1;
  waw (BELOW, ABOVE, c, d, N);
  if (*x != 1)
    __builtin_abort ();

  return 0;
}

/* { dg-final { scan-tree-dump-times "LOOP VECTORIZED" 3 "vect" } } */
