/* An index with a constant multiplier doesn't tell two accesses apart if the
   multiplier wraps the address: with an 8-byte element, (1L << 61) + 1 moves
   the address by one element, so each iteration reads what the previous one
   wrote.  */
/* { dg-do run } */
/* { dg-require-effective-target lp64 } */
/* { dg-additional-options "-O3 -fno-strict-aliasing -fno-strict-overflow -fno-vect-cost-model" } */

#define K ((1L << 61) + 1)

extern void abort (void);

struct e { int x, y; };
struct w { long n; struct e d[]; };

static char mem[4096] __attribute__ ((aligned (64)));

__attribute__ ((noinline, noclone)) void
fields (struct w *f, long n)
{
  for (long i = 0; i < n; i++)
    f->d[K * i + 1].x = f->d[K * i].x + 1;
}

struct l { long n; long d[]; };

__attribute__ ((noinline, noclone)) void
whole (struct l *f)
{
  for (long i = 0; i < 16; i++)
    f->d[K * i + 1] = f->d[K * i] + 1;
}

int
main (void)
{
  struct w *f = (struct w *) mem;
  fields (f, 16);
  if (f->d[16].x != 16 || f->d[8].x != 8)
    abort ();

  __builtin_memset (mem, 0, sizeof (mem));

  struct l *g = (struct l *) mem;
  whole (g);
  if (g->d[16] != 16 || g->d[8] != 8)
    abort ();
  return 0;
}
