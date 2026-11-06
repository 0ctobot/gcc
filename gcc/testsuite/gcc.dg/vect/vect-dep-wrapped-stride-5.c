/* { dg-additional-options "-O3 -fno-strict-aliasing -fno-strict-overflow -fno-vect-cost-model" } */
/* { dg-require-effective-target int32plus } */

/* Addresses can wrap here, and nothing relates the two indices of a loop to each
   other.  Structures whose size is a power of two are still either the same one
   or fully disjoint for any two indices since the low bits of an address don't
   depend on the index.  For other sizes that takes indices narrow enough that
   they can't wrap around to a partial overlap once they're scaled, which a long
   index isn't, leaving just that loop with a runtime check.  */

#include "tree-vect.h"

struct p2 { int x, y; };
struct s3 { int x, y, z; };

#define M 8192

struct p2 ga[M], ea[M];
struct s3 gs[M], es[M];

#define DEF(NAME, TYPE, A, E, WRITE, READ)				\
  __attribute__ ((noipa)) void						\
  NAME (TYPE n, TYPE d)							\
  {									\
    for (TYPE i = 8; i < n; i++)					\
      A[WRITE].x = A[READ].y;						\
  }									\
  __attribute__ ((noipa)) void						\
  NAME##_ref (TYPE n, TYPE d)						\
  {									\
    _Pragma ("GCC novector")						\
    for (TYPE i = 8; i < n; i++)					\
      E[WRITE].x = E[READ].y;						\
  }

DEF (p2_long, long, ga, ea, i, i + d)
DEF (p2_int, int, ga, ea, i, i + d)
DEF (s3_int, int, gs, es, i, i + d)
DEF (s3_int_dist, int, gs, es, i + d, i + d - 2)
DEF (s3_uint_dist, unsigned int, gs, es, i + d, i + d + 1)
DEF (s3_long, long, gs, es, i, i + d)

static void
init (void)
{
#pragma GCC novector
  for (int i = 0; i < M; i++)
    {
      ga[i].x = ea[i].x = i;
      ga[i].y = ea[i].y = 2 * i;
      gs[i].x = es[i].x = i;
      gs[i].y = es[i].y = 3 * i;
      gs[i].z = es[i].z = 7;
    }
}

#define RUN(NAME, D)							\
  do									\
    {									\
      init ();								\
      NAME (4096, D);							\
      NAME##_ref (4096, D);						\
      if (__builtin_memcmp (ga, ea, sizeof (ga))			\
	  || __builtin_memcmp (gs, es, sizeof (gs)))			\
	__builtin_abort ();						\
    }									\
  while (0)

int
main (void)
{
  check_vect ();

  RUN (p2_long, -2);
  RUN (p2_int, -2);
  RUN (s3_int, -2);
  RUN (s3_int_dist, 8);
  RUN (s3_uint_dist, 8);
  RUN (s3_long, -2);
  return 0;
}

/* { dg-final { scan-tree-dump-times "vectorized 1 loops" 6 "vect" { target aarch64*-*-* } } } */
/* { dg-final { scan-tree-dump-times "loop versioned for vectorization because of possible aliasing" 1 "vect" { target aarch64*-*-* } } } */
