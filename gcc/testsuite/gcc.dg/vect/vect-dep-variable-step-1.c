/* A dimension whose indices are the same in both references only tells
   iterations apart if its step isn't zero.  With a row step of M = 0 known
   only at run time, f->d[M * j] is the same row for every J, so each
   iteration reads what the previous one wrote.  */
/* { dg-do run } */
/* { dg-additional-options "-O3 -fno-vect-cost-model" } */

extern void abort (void);

struct r4 { long n; long d[][4]; };

static long mem[64] __attribute__ ((aligned (64)));

__attribute__ ((noipa)) void
same_row (struct r4 *f, long n, long m)
{
  for (long j = 0; j < n; j++)
    f->d[m * j][j + 1] = f->d[m * j][j] + 1;
}

int
main (void)
{
  struct r4 *f = (struct r4 *) mem;

  same_row (f, 3, 0);
  if (f->d[0][3] != 3)
    abort ();
  return 0;
}
