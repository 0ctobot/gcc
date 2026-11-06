/* ACLE defines the signed integer arithmetic intrinsics to wrap on overflow.
   If arm_neon.h implements them with C operators on signed types, the compiler
   may assume they don't overflow and fold x + 1 > x to true, (x * 2) / 2 to x
   or -x >> (bits - 1) to -(x > 0).  Each function below holds such an
   expression for one intrinsic, kept out of line so that it's folded with its
   argument unknown.  */
/* { dg-do run } */
/* { dg-options "-O2" } */

#include <arm_neon.h>
#include <stdint.h>

extern void abort (void);

#define NOIPA __attribute__ ((noipa))

/* Q suffix, element suffix, vector type, element maximum and minimum, and a
   value that overflows when doubled.  */
#define WIDTH_8(X) \
  X (, s8, int8x8_t, INT8_MAX, INT8_MIN, 0x40) \
  X (q, s8, int8x16_t, INT8_MAX, INT8_MIN, 0x40)
#define WIDTH_16(X) \
  X (, s16, int16x4_t, INT16_MAX, INT16_MIN, 0x4000) \
  X (q, s16, int16x8_t, INT16_MAX, INT16_MIN, 0x4000)
#define WIDTH_32(X) \
  X (, s32, int32x2_t, INT32_MAX, INT32_MIN, 0x40000000) \
  X (q, s32, int32x4_t, INT32_MAX, INT32_MIN, 0x40000000)
#define WIDTH_64(X) \
  X (, s64, int64x1_t, INT64_MAX, INT64_MIN, 0x4000000000000000) \
  X (q, s64, int64x2_t, INT64_MAX, INT64_MIN, 0x4000000000000000)

/* The same, with the Q suffix of the lane operand after that of the result.  */
#define LANES(X) \
  X (, , s16, int16x4_t, INT16_MAX, INT16_MIN, 0x4000) \
  X (, q, s16, int16x4_t, INT16_MAX, INT16_MIN, 0x4000) \
  X (q, , s16, int16x8_t, INT16_MAX, INT16_MIN, 0x4000) \
  X (q, q, s16, int16x8_t, INT16_MAX, INT16_MIN, 0x4000) \
  X (, , s32, int32x2_t, INT32_MAX, INT32_MIN, 0x40000000) \
  X (, q, s32, int32x2_t, INT32_MAX, INT32_MIN, 0x40000000) \
  X (q, , s32, int32x4_t, INT32_MAX, INT32_MIN, 0x40000000) \
  X (q, q, s32, int32x4_t, INT32_MAX, INT32_MIN, 0x40000000)

#define DUP(Q, S, V) vdup##Q##_n_##S (V)

/* Returns the result of x + 1 > x, x - 1 < x or -x > -y.  */
#define DEF_ADD(Q, S, T, MAX, MIN, BIG) \
  NOIPA int test_vadd##Q##_##S (T a) \
  { \
    return vcgt##Q##_##S (vadd##Q##_##S (a, DUP (Q, S, 1)), a)[0] != 0; \
  }
#define DEF_SUB(Q, S, T, MAX, MIN, BIG) \
  NOIPA int test_vsub##Q##_##S (T a) \
  { \
    return vclt##Q##_##S (vsub##Q##_##S (a, DUP (Q, S, 1)), a)[0] != 0; \
  }
#define DEF_NEG(Q, S, T, MAX, MIN, BIG) \
  NOIPA int test_vneg##Q##_##S (T a, T b) \
  { \
    return vcgt##Q##_##S (vneg##Q##_##S (a), vneg##Q##_##S (b))[0] != 0; \
  }

/* Returns the result of -x >> (bits - 1).  */
#define DEF_NEG_SHR(Q, S, T, MAX, MIN, BIG) \
  NOIPA T test_vneg_shr##Q##_##S (T a) \
  { \
    return vshr##Q##_n_##S (vneg##Q##_##S (a), sizeof (a[0]) * 8 - 1); \
  }

/* Returns the result of (x * 2) / 2.  */
#define DEF_MUL(Q, S, T, MAX, MIN, BIG) \
  NOIPA T test_vmul##Q##_##S (T a) \
  { \
    return vmul##Q##_##S (a, DUP (Q, S, 2)) / DUP (Q, S, 2); \
  }
#define DEF_MUL_N(Q, S, T, MAX, MIN, BIG) \
  NOIPA T test_vmul##Q##_n_##S (T a) \
  { \
    return vmul##Q##_n_##S (a, 2) / DUP (Q, S, 2); \
  }
#define DEF_LANE(Q, LQ, S, T, MAX, MIN, BIG) \
  NOIPA T test_vmul##Q##_lane##LQ##_##S (T a) \
  { \
    return vmul##Q##_lane##LQ##_##S (a, DUP (LQ, S, 2), 1) / DUP (Q, S, 2); \
  } \
  NOIPA int test_vmla##Q##_lane##LQ##_##S (T a) \
  { \
    return vcgt##Q##_##S (vmla##Q##_lane##LQ##_##S (a, DUP (Q, S, 1), \
						    DUP (LQ, S, 1), 1), \
			  a)[0] != 0; \
  } \
  NOIPA int test_vmls##Q##_lane##LQ##_##S (T a) \
  { \
    return vclt##Q##_##S (vmls##Q##_lane##LQ##_##S (a, DUP (Q, S, 1), \
						    DUP (LQ, S, 1), 1), \
			  a)[0] != 0; \
  }

WIDTH_8 (DEF_ADD) WIDTH_16 (DEF_ADD) WIDTH_32 (DEF_ADD) WIDTH_64 (DEF_ADD)
WIDTH_8 (DEF_SUB) WIDTH_16 (DEF_SUB) WIDTH_32 (DEF_SUB) WIDTH_64 (DEF_SUB)
WIDTH_8 (DEF_NEG) WIDTH_16 (DEF_NEG) WIDTH_32 (DEF_NEG) WIDTH_64 (DEF_NEG)
WIDTH_8 (DEF_NEG_SHR) WIDTH_16 (DEF_NEG_SHR)
WIDTH_32 (DEF_NEG_SHR) WIDTH_64 (DEF_NEG_SHR)
WIDTH_8 (DEF_MUL) WIDTH_16 (DEF_MUL) WIDTH_32 (DEF_MUL)
WIDTH_16 (DEF_MUL_N) WIDTH_32 (DEF_MUL_N)
LANES (DEF_LANE)

NOIPA int
test_vaddd_s64 (int64_t a)
{
  return vaddd_s64 (a, 1) > a;
}

NOIPA int
test_vsubd_s64 (int64_t a)
{
  return vsubd_s64 (a, 1) < a;
}

#define CHECK_ADD(Q, S, T, MAX, MIN, BIG) \
  if (test_vadd##Q##_##S (DUP (Q, S, MAX))) \
    abort ();
#define CHECK_SUB(Q, S, T, MAX, MIN, BIG) \
  if (test_vsub##Q##_##S (DUP (Q, S, MIN))) \
    abort ();
#define CHECK_NEG(Q, S, T, MAX, MIN, BIG) \
  if (test_vneg##Q##_##S (DUP (Q, S, MIN), DUP (Q, S, 0))) \
    abort ();
#define CHECK_NEG_SHR(Q, S, T, MAX, MIN, BIG) \
  if (test_vneg_shr##Q##_##S (DUP (Q, S, MIN))[0] != -1) \
    abort ();
#define CHECK_MUL(Q, S, T, MAX, MIN, BIG) \
  if (test_vmul##Q##_##S (DUP (Q, S, BIG))[0] != -BIG) \
    abort ();
#define CHECK_MUL_N(Q, S, T, MAX, MIN, BIG) \
  if (test_vmul##Q##_n_##S (DUP (Q, S, BIG))[0] != -BIG) \
    abort ();
#define CHECK_LANE(Q, LQ, S, T, MAX, MIN, BIG) \
  if (test_vmul##Q##_lane##LQ##_##S (DUP (Q, S, BIG))[0] != -BIG) \
    abort (); \
  if (test_vmla##Q##_lane##LQ##_##S (DUP (Q, S, MAX))) \
    abort (); \
  if (test_vmls##Q##_lane##LQ##_##S (DUP (Q, S, MIN))) \
    abort ();

int
main (void)
{
  WIDTH_8 (CHECK_ADD) WIDTH_16 (CHECK_ADD)
  WIDTH_32 (CHECK_ADD) WIDTH_64 (CHECK_ADD)
  WIDTH_8 (CHECK_SUB) WIDTH_16 (CHECK_SUB)
  WIDTH_32 (CHECK_SUB) WIDTH_64 (CHECK_SUB)
  WIDTH_8 (CHECK_NEG) WIDTH_16 (CHECK_NEG)
  WIDTH_32 (CHECK_NEG) WIDTH_64 (CHECK_NEG)
  WIDTH_8 (CHECK_NEG_SHR) WIDTH_16 (CHECK_NEG_SHR)
  WIDTH_32 (CHECK_NEG_SHR) WIDTH_64 (CHECK_NEG_SHR)
  WIDTH_8 (CHECK_MUL) WIDTH_16 (CHECK_MUL) WIDTH_32 (CHECK_MUL)
  WIDTH_16 (CHECK_MUL_N) WIDTH_32 (CHECK_MUL_N)
  LANES (CHECK_LANE)

  if (test_vaddd_s64 (INT64_MAX))
    abort ();
  if (test_vsubd_s64 (INT64_MIN))
    abort ();

  return 0;
}
