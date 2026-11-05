/* PR tree-optimization/128351 */
/* { dg-add-options vect_early_break } */
/* { dg-require-effective-target vect_early_break_hw } */
/* { dg-require-effective-target vect_int } */
/* { dg-additional-options "-O3" } */
/* A prolog chosen at run time is what lets the wrapped count go wrong.  Loops
   without an early break only get one on AArch64 when alignment is enforced.  */
/* { dg-additional-options "-mstrict-align" { target aarch64*-*-* } } */

/* { dg-final { scan-tree-dump "LOOP VECTORIZED" "vect" { target aarch64*-*-* } } } */

#include "tree-vect.h"

/* A narrow counter tested with != covers the whole range of its type when the
   limit is 0.  Check small counts and that full range, with the prolog taken and
   not taken, for loops with and without an early break.  */

#define OFFS 17
#define MATCH 77

#define DEF(T, C)							\
  __attribute__ ((noipa)) unsigned long					\
  find_##T##_##C (const T *a, C n)					\
  {									\
    C i = 0;								\
    const T *p = a;							\
									\
    do									\
      {									\
	if (*p == MATCH)						\
	  break;							\
	p++;								\
      }									\
    while (++i != n);							\
    return p - a;							\
  }									\
									\
  __attribute__ ((noipa)) void						\
  inc_##T##_##C (T *p, C n)						\
  {									\
    C i = 0;								\
									\
    do									\
      *p++ += 1;							\
    while (++i != n);							\
  }									\
									\
  __attribute__ ((noipa)) unsigned int					\
  sum_##T##_##C (const T *p, C n)					\
  {									\
    C i = 0;								\
    unsigned int s = 0;							\
									\
    do									\
      s += *p++;							\
    while (++i != n);							\
    return s;								\
  }

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

DEF (u8, u8)
DEF (u32, u8)
DEF (u8, u16)

#define TEST(T, C, COUNT)						\
  do									\
    {									\
      T *q = (T *) buf + off;						\
      __builtin_memset (buf, 0, sizeof (buf));				\
      inc_##T##_##C (q, (C) (COUNT));					\
      /* Every element of the count got incremented and no others.  */	\
      _Pragma ("GCC novector")						\
      for (unsigned long i = 0; i < (COUNT) + OFFS; i++)		\
	if (q[i] != (i < (COUNT)))					\
	  abort ();							\
      if (sum_##T##_##C (q, (C) (COUNT)) != (COUNT))			\
	abort ();							\
      if (find_##T##_##C (q, (C) (COUNT)) != (COUNT))			\
	abort ();							\
      /* Stopping at the last element takes the other exit.  */		\
      q[(COUNT) - 1] = MATCH;						\
      if (find_##T##_##C (q, (C) (COUNT)) != (COUNT) - 1)		\
	abort ();							\
    }									\
  while (0)

static unsigned char buf[(65536 + 2 * OFFS) * sizeof (u32)]
  __attribute__ ((aligned (64)));

int
main (void)
{
  check_vect ();

  for (int off = 0; off < OFFS; off++)
    {
#pragma GCC novector
      for (unsigned int n = 1; n <= 256; n += n < 16 || n >= 250 ? 1 : 13)
	{
	  TEST (u8, u8, n);
	  TEST (u32, u8, n);
	}
      TEST (u8, u16, 65536u);
    }
  return 0;
}
