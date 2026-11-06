/* { dg-require-effective-target lp64 } */
/* { dg-additional-options "-O3 -fno-strict-aliasing -fno-strict-overflow -fno-vect-cost-model" } */

/* 24 * K is 8 modulo 1 << 64, which means that p[i * K].x and p[i * K].y walk
   memory by 8 bytes and that p[(i + 1) * K].x is the same object as p[i * K].y.
   The fields being different doesn't make these references independent.  */

#include "tree-vect.h"

struct s { long x, y, z; };
struct w { long pad; struct s e[1]; };
struct n { struct { long u, v; } in; long z; };
struct arr { long a[3]; };

#define K 768614336404564651L
#define N 40

long m[200];

#define LOOP(BODY)				\
  long t = 0;					\
  for (long i = 0; i < n; i++)			\
    {						\
      BODY;					\
    }						\
  return t

__attribute__ ((noipa)) long
f (struct s *p, long n)
{
  LOOP (p[i * K].x = i + 1; t += p[i * K].y);
}

__attribute__ ((noipa)) long
g (struct s *p, long n, long k)
{
  LOOP (p[i * k].x = i + 1; t += p[i * k].y);
}

__attribute__ ((noipa)) long
r (struct s *p, long n)
{
  LOOP (t += p[-i * K].y; p[-i * K].x = i + 1);
}

__attribute__ ((noipa)) long
rv (struct s *p, long n, long k)
{
  LOOP (t += p[i * k].y; p[i * k].x = i + 1);
}

__attribute__ ((noipa)) long
fa (long n)
{
  LOOP (((struct s *) m)[i * K].x = i + 1; t += ((struct s *) m)[i * K].y);
}

__attribute__ ((noipa)) long
fw (struct w *p, long n)
{
  LOOP (p->e[i * K].x = i + 1; t += p->e[i * K].y);
}

__attribute__ ((noipa)) long
fn (struct n *p, long n)
{
  LOOP (p[i * K].in.u = i + 1; t += p[i * K].in.v);
}

__attribute__ ((noipa)) long
fi (struct arr *p, long n)
{
  LOOP (p[i * K].a[0] = i + 1; t += p[i * K].a[1]);
}

/* The same overlap, without any wrapping.  */

__attribute__ ((noipa)) long
fo (char *b, long n)
{
  LOOP (struct s *p = (struct s *) (b + i * 8); p->x = i + 1; t += p->y);
}

__attribute__ ((noipa)) long
fv (char *b, long n, long k)
{
  LOOP (struct s *p = (struct s *) (b + i * k); p->x = i + 1; t += p->y);
}

static void
init (void)
{
#pragma GCC novector
  for (int i = 0; i < 200; i++)
    m[i] = 1000 + i;
}

#define CHECK(CALL, VAL)			\
  do						\
    {						\
      init ();					\
      if ((CALL) != (VAL))			\
	__builtin_abort ();			\
    }						\
  while (0)

int
main (void)
{
  check_vect ();

  CHECK (f ((struct s *) (m + 10), N), 41220);
  CHECK (g ((struct s *) (m + 10), N, K), 41220);
  CHECK (r ((struct s *) (m + 60), N), 1841);
  CHECK (rv ((struct s *) (m + 60), N, -K), 1841);
  CHECK (fa (N), 40820);
  CHECK (fw ((struct w *) m, N), 40860);
  CHECK (fn ((struct n *) m, N), 40820);
  CHECK (fi ((struct arr *) m, N), 40820);
  CHECK (fo ((char *) m, N), 40820);
  CHECK (fv ((char *) m, N, 8), 40820);
  return 0;
}
