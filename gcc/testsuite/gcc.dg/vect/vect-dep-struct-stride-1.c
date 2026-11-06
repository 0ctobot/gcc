/* { dg-require-effective-target lp64 } */
/* { dg-additional-options "-O3 -fno-strict-aliasing -fno-strict-overflow -fno-vect-cost-model" } */

/* Stepping by 8 bytes over a 24-byte structure makes the structures of
   consecutive iterations overlap, which means that p->x of one iteration is the
   same object as p->y of the previous one.  The fields being different doesn't
   make these references independent.  */

#include "tree-vect.h"

struct s { long x, y, z; };

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

  CHECK (fo ((char *) m, N), 40820);
  CHECK (fv ((char *) m, N, 8), 40820);
  return 0;
}
