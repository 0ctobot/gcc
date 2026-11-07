/* Same as vect-dep-wrapped-stride-8.c same_row, but with a row step that's
   only known at run time.  Knowing that the step isn't zero doesn't help
   either, since 1L << 59 rows of four longs add up to 1 << 64 bytes and
   every iteration still stays in the same row.  */
/* { dg-do run } */
/* { dg-require-effective-target lp64 } */
/* { dg-additional-options "-O3 -fno-strict-aliasing -fno-strict-overflow -fno-vect-cost-model" } */

#define C59 (1L << 59)

extern void abort (void);

struct r4 { long n; long d[][4]; };

static long mem[64] __attribute__ ((aligned (64)));

__attribute__ ((noipa)) void
any_row (struct r4 *f, long n, long m)
{
  for (long j = 0; j < n; j++)
    f->d[m * j][j + 1] = f->d[m * j][j] + 1;
}

__attribute__ ((noipa)) void
nonzero_row (struct r4 *f, long n, long x)
{
  long m = x | C59;

  for (long j = 0; j < n; j++)
    f->d[m * j][j + 1] = f->d[m * j][j] + 1;
}

int
main (void)
{
  struct r4 *f = (struct r4 *) mem;

  any_row (f, 3, C59);
  if (f->d[0][3] != 3)
    abort ();

  __builtin_memset (mem, 0, sizeof mem);
  nonzero_row (f, 3, 0);
  if (f->d[0][3] != 3)
    abort ();
  return 0;
}
