/* With -fwrapv-pointer, array elements whose indices differ can be the same
   element, or overlap, once the distance between them wraps the address.
   With rows of four longs, f->d[1L << 59] is f->d[0], and with a 24-byte
   element, f->d[i + K].a is f->d[i].b for the K below.  The alias oracle
   must not take such references apart.  */
/* { dg-do run } */
/* { dg-require-effective-target lp64 } */
/* { dg-additional-options "-fwrapv-pointer -fno-strict-aliasing" } */

#define C59 (1L << 59)

/* Rows of four longs are 32 bytes, so f->d[C59] is f->d[0].  */
struct r4 { long n; long d[][4]; };

/* 24-byte elements, 24 * K being 8 more than 1 << 64.  */
struct s3 { long a, b, c; };
struct r3 { long n; struct s3 d[]; };
#define K (((1L << 61) + 1) / 3)

static long mem[64] __attribute__ ((aligned (64)));

__attribute__ ((noipa)) void
straight (struct r4 *f)
{
  f->d[C59][1] = f->d[0][0] + 1;
  f->d[C59][2] = f->d[0][1] + 1;
}

__attribute__ ((noipa)) long
fre (struct r4 *f)
{
  f->d[0][1] = 1;
  f->d[C59][1] = 2;
  return f->d[0][1];
}

__attribute__ ((noipa)) long
lim (struct r4 *f, long n)
{
  long s = 0;
  for (long i = 0; i < n; i++)
    {
      f->d[C59][1] = i;
      s += f->d[0][1];
    }
  return s;
}

__attribute__ ((noipa)) long
var (struct r3 *f, long i, long j)
{
  f->d[i].b = 1;
  f->d[j].a = 2;
  return f->d[i].b;
}

int
main (void)
{
  struct r4 *g = (struct r4 *) mem;
  straight (g);
  if (g->d[0][1] != 1 || g->d[0][2] != 2)
    __builtin_abort ();
  __builtin_memset (mem, 0, sizeof mem);
  if (fre (g) != 2)
    __builtin_abort ();
  __builtin_memset (mem, 0, sizeof mem);
  if (lim (g, 4) != 6)
    __builtin_abort ();
  __builtin_memset (mem, 0, sizeof mem);
  if (var ((struct r3 *) mem, 1, 1 + K) != 2)
    __builtin_abort ();
  return 0;
}
