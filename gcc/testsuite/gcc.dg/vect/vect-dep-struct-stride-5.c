/* { dg-additional-options "-O3 -fno-strict-aliasing -fno-strict-overflow -fno-vect-cost-model" } */
/* { dg-require-effective-target int32plus } */

/* Nothing relates the two indices of a loop to each other, and their difference
   doesn't fold under -fno-strict-overflow even when it's a constant.  An array
   index counts whole structures, so any two indices select either the same
   structure or fully disjoint ones, and the fields of these loops stay
   independent without a runtime check.  */

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
/* { dg-final { scan-tree-dump-not "loop versioned for vectorization because of possible aliasing" "vect" { target aarch64*-*-* } } } */
