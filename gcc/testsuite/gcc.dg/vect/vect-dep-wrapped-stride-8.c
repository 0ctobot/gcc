/* An outer dimension whose indices are the same in both references still
   doesn't tell iterations apart if its step wraps the address: with a row of
   four 8-byte elements, a step of 1L << 59 rows moves the address by 1 << 64
   bytes, so every iteration stays in the same row and reads what the previous
   one wrote.  */
/* { dg-do run } */
/* { dg-require-effective-target lp64 } */
/* { dg-additional-options "-O3 -fno-strict-aliasing -fno-strict-overflow -fno-vect-cost-model" } */

#define C59 (1L << 59)

extern void abort (void);

struct r4 { long n; long d[][4]; };

static long mem[64] __attribute__ ((aligned (64)));

__attribute__ ((noipa)) void
same_row (struct r4 *f, long n)
{
  for (long j = 0; j < n; j++)
    f->d[C59 * j][j + 1] = f->d[C59 * j][j] + 1;
}

/* With both references the same, every iteration updates one element.  */

__attribute__ ((noipa)) void
same_element (struct r4 *f, long n)
{
  for (long j = 0; j < n; j++)
    f->d[C59 * j][0] = f->d[C59 * j][0] + 1;
}

int
main (void)
{
  struct r4 *f = (struct r4 *) mem;

  same_row (f, 3);
  if (f->d[0][3] != 3)
    abort ();

  __builtin_memset (mem, 0, sizeof mem);
  same_element (f, 16);
  if (f->d[0][0] != 16)
    abort ();
  return 0;
}
