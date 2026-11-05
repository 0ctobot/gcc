/* PR tree-optimization/128403 */
/* { dg-do run { target { *-*-linux* && aarch64_sve_hw } } } */
/* { dg-options "-O2 -ftree-vectorize -march=armv8-a+sve2 -fno-vect-cost-model -fno-strict-overflow -fdump-tree-vect-details" } */

/* The multiply-high results here need one bit more than the operands have and
   their users keep that bit, so .MULHS and .MULHRS on the operand type would
   drop it.  The over-widening pattern narrows each shift first, which used to
   hide the users' precision from the multiply-high pattern.

   The aarch64_sve2_hw probe doesn't enable SVE2 when it builds its program, so
   it only passes when the compiler defaults to SVE2.  Check for SVE2 at run
   time instead, in order to actually run this on an SVE2 target.  */

#include <sys/auxv.h>

#define N 32

#define TEST(NAME, OUT_TYPE, IN_TYPE, EXPR)				\
  OUT_TYPE NAME##_out[N];						\
  IN_TYPE NAME##_a[N], NAME##_b[N];					\
									\
  __attribute__ ((noipa)) void						\
  NAME##_vect (void)							\
  {									\
    for (int i = 0; i < N; i++)						\
      {									\
	IN_TYPE a = NAME##_a[i], b = NAME##_b[i];			\
	NAME##_out[i] = EXPR;						\
      }									\
  }									\
									\
  __attribute__ ((noipa, optimize ("no-tree-vectorize"))) OUT_TYPE	\
  NAME##_scalar (IN_TYPE a, IN_TYPE b)					\
  {									\
    return EXPR;							\
  }									\
									\
  __attribute__ ((noipa)) void						\
  NAME##_check (IN_TYPE first)						\
  {									\
    for (int i = 0; i < N; i++)						\
      {									\
	NAME##_a[i] = i ? (IN_TYPE) (i * 0x01010101u) : first;		\
	NAME##_b[i] = i ? (IN_TYPE) (i * 0x11111111u) : first;		\
      }									\
    NAME##_vect ();							\
    for (int i = 0; i < N; i++)						\
      if (NAME##_out[i] != NAME##_scalar (NAME##_a[i], NAME##_b[i]))	\
	__builtin_abort ();						\
  }

/* The shift gets narrowed to 16 bits and all of them are needed.  */
TEST (u8_mulhs, unsigned short, unsigned char, ((int) a * (int) b) >> 7)
TEST (s8_mulhs, short, signed char, ((int) a * (int) b) >> 7)
TEST (u8_mulhrs, unsigned short, unsigned char,
      (((int) a * (int) b >> 6) + 1) >> 1)
TEST (s8_mulhrs, short, signed char, (((int) a * (int) b >> 6) + 1) >> 1)

/* The shift gets narrowed to 32 bits and all of them are needed.  */
TEST (u16_mulhs, unsigned int, unsigned short,
      ((long long) a * (long long) b) >> 15)
TEST (s16_mulhs, int, short, ((long long) a * (long long) b) >> 15)
TEST (u16_mulhrs, unsigned int, unsigned short,
      (((long long) a * (long long) b >> 14) + 1) >> 1)
TEST (s16_mulhrs, int, short, (((long long) a * (long long) b >> 14) + 1) >> 1)

int
main (void)
{
  if (!(getauxval (AT_HWCAP2) & HWCAP2_SVE2))
    return 0;

  u8_mulhs_check (0xff);
  s8_mulhs_check (-128);
  u8_mulhrs_check (0xff);
  s8_mulhrs_check (-128);
  u16_mulhs_check (0xffff);
  s16_mulhs_check (-32768);
  u16_mulhrs_check (0xffff);
  s16_mulhrs_check (-32768);
  return 0;
}

/* { dg-final { scan-tree-dump-not "\\.MULHR?S" "vect" } } */
