/* PR tree-optimization/128403 */
/* -fno-strict-overflow lets reassociation reorder the signed int sums too.  */
/* { dg-additional-options "-fno-strict-overflow" } */

#include "tree-vect.h"

#define N 32

/* Each loop computes a rounded average whose inputs don't all fit in the type
   of the first input.  Reassociation turns (x + y) + 1 into y + (x + 1), and
   the vectorizer must not treat the type of x as the common type of both.  */

#define TEST(NAME, OUT_TYPE, A_TYPE, B_TYPE, SUM_TYPE)			\
  OUT_TYPE NAME##_out[N];						\
  A_TYPE NAME##_a[N];							\
  B_TYPE NAME##_b[N];							\
									\
  __attribute__ ((noipa)) void						\
  NAME##_vect (void)							\
  {									\
    for (int i = 0; i < N; i++)						\
      NAME##_out[i] = ((SUM_TYPE) NAME##_a[i] + (SUM_TYPE) NAME##_b[i]	\
		       + 1) >> 1;					\
  }									\
									\
  __attribute__ ((noipa)) OUT_TYPE					\
  NAME##_scalar (A_TYPE a, B_TYPE b)					\
  {									\
    return ((SUM_TYPE) a + (SUM_TYPE) b + 1) >> 1;			\
  }									\
									\
  __attribute__ ((noipa)) void						\
  NAME##_check (void)							\
  {									\
    /* Mix small and large values of both signs.  */			\
    for (int i = 0; i < N; i++)						\
      {									\
	NAME##_a[i] = (i & 1) ? -1 - i : i * 0x01010101u;		\
	NAME##_b[i] = (i & 2) ? -1 - i : i * 0x11111111u;		\
      }									\
    NAME##_vect ();							\
    for (int i = 0; i < N; i++)						\
      if (NAME##_out[i] != NAME##_scalar (NAME##_a[i], NAME##_b[i]))	\
	__builtin_abort ();						\
  }

/* The reported testcase, with the sum truncated before the shift.  */
unsigned short pr_a[N];
signed char pr_b[N];
unsigned short pr_out[N];

__attribute__ ((noipa)) void
pr_vect (void)
{
  for (int i = 0; i < N; i++)
    pr_out[i] = (unsigned) ((unsigned long long) pr_a[i] + pr_b[i] + 1) >> 1;
}

TEST (u16_s8, unsigned short, unsigned short, signed char, unsigned int)
TEST (s8_u16, unsigned short, signed char, unsigned short, unsigned int)
TEST (u8_s8, unsigned char, unsigned char, signed char, int)
TEST (s8_u8, unsigned char, signed char, unsigned char, int)
TEST (s16_u16, unsigned short, short, unsigned short, unsigned long long)
TEST (s8_s16, short, signed char, short, unsigned long long)

int
main (void)
{
  check_vect ();

  pr_b[1] = -1;
  pr_vect ();
#pragma GCC novector
  for (int i = 0; i < N; i++)
    if (pr_out[i] != 0)
      __builtin_abort ();

  u16_s8_check ();
  s8_u16_check ();
  u8_s8_check ();
  s8_u8_check ();
  s16_u16_check ();
  s8_s16_check ();

  return 0;
}

/* The operands of s8_s16 all fit in short, which the pattern can still use.  */
/* { dg-final { scan-tree-dump "vect_recog_average_pattern: detected" "vect" { target aarch64*-*-* } } } */
