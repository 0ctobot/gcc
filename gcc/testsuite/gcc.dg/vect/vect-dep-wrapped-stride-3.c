/* { dg-require-effective-target lp64 } */
/* { dg-additional-options "-O3 -fno-strict-aliasing -fno-strict-overflow -fno-vect-cost-model" } */

/* A stride of 16 bytes keeps 12-byte objects apart and so does a stride of 12
   bytes, for different reasons that don't combine: one reference's object at
   offset 16 overlaps the other's at offset 12.  Two references with the same
   stride also overlap if their initial offsets aren't a multiple of it apart.
   The fields being different doesn't make these references independent.  */

#include "tree-vect.h"

struct s { int x, y, z; };

#define N 40
#define SIZE 256

#define DEF(NAME, BODY)				\
  __attribute__ ((noipa)) void			\
  NAME (char *b, long n)			\
  {						\
    for (long i = 0; i < n; i++)		\
      {						\
	BODY;					\
      }						\
  }						\
						\
  __attribute__ ((noipa)) void			\
  NAME##_ref (char *b, long n)			\
  {						\
    _Pragma ("GCC novector")			\
    for (long i = 0; i < n; i++)		\
      {						\
	BODY;					\
      }						\
  }

/* Different strides, one for each rule.  */
DEF (mix,
     struct s *p = (struct s *) (b + i * 16);
     struct s *q = (struct s *) (b + i * 12);
     p->x = q->y + 1)

DEF (mix_z,
     struct s *p = (struct s *) (b + i * 16);
     struct s *q = (struct s *) (b + i * 12);
     p->z = q->x + 1)

/* The same stride, with initial offsets that aren't a multiple of it apart.  */
DEF (init_off,
     struct s *p = (struct s *) (b + 16 + i * 16);
     struct s *q = (struct s *) (b + 4 + i * 16);
     p->y = q->x + 1)

static long a[SIZE], c[SIZE];

static void
init (void)
{
#pragma GCC novector
  for (int i = 0; i < SIZE; i++)
    a[i] = c[i] = i * 0x0101010101010101L;
}

#define CHECK(NAME)					\
  do							\
    {							\
      init ();						\
      NAME ((char *) a, N);				\
      NAME##_ref ((char *) c, N);			\
      if (__builtin_memcmp (a, c, sizeof (a)) != 0)	\
	__builtin_abort ();				\
    }							\
  while (0)

int
main (void)
{
  check_vect ();

  CHECK (mix);
  CHECK (mix_z);
  CHECK (init_off);
  return 0;
}
