/* Indices that differ by a constant don't tell two accesses apart if the
   difference wraps the address: with an 8-byte element, an offset of
   (1L << 61) + 1 moves the address by one element, and with a row of four
   8-byte elements, an outer offset of 1L << 59 doesn't move it at all.  Each
   iteration then reads what the previous one wrote.  */
/* { dg-do run } */
/* { dg-require-effective-target lp64 } */
/* { dg-additional-options "-O3 -fno-strict-aliasing -fno-strict-overflow -fno-vect-cost-model" } */

#define C1 ((1L << 61) + 1)
#define C3 ((1L << 61) + 3)
#define C59 (1L << 59)

extern void abort (void);

struct l { long n; long d[]; };
struct r4 { long n; long d[][4]; };

static char mem[4096] __attribute__ ((aligned (64)));

__attribute__ ((noipa)) void
fixed_count (struct l *f)
{
  for (long i = 0; i < 16; i++)
    f->d[i + C1] = f->d[i] + 1;
}

__attribute__ ((noipa)) void
gcd (struct l *f, long n)
{
  for (long i = 0; i < n; i++)
    f->d[3 * i + C3] = f->d[3 * i] + 1;
}

__attribute__ ((noipa)) void
outer (struct r4 *f, long n)
{
  for (long j = 0; j < n; j++)
    f->d[C59][j + 1] = f->d[0][j] + 1;
}

int
main (void)
{
  struct l *f = (struct l *) mem;
  struct r4 *g = (struct r4 *) mem;

  fixed_count (f);
  if (f->d[16] != 16)
    abort ();

  __builtin_memset (mem, 0, sizeof mem);
  gcd (f, 16);
  if (f->d[48] != 16)
    abort ();

  __builtin_memset (mem, 0, sizeof mem);
  outer (g, 3);
  if (g->d[0][3] != 3)
    abort ();

  return 0;
}
