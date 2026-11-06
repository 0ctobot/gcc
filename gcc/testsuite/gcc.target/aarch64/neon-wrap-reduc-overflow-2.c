/* The vaddv intrinsics wrap on overflow, even for a signed element type and
   with -ftrapv.  REDUC_PLUS (a + k) mustn't be folded to REDUC_PLUS (a) +
   REDUC_PLUS (k) for a constant k with -ftrapv, since that addition would
   trap when the sum overflows.  No element of a + k overflows here, only the
   sum does.  */
/* { dg-do run } */
/* { dg-options "-O2 -ftrapv" } */

#include <arm_neon.h>
#include <stdint.h>

extern void abort (void);

#define NOIPA __attribute__ ((noipa))

/* Q suffix, element suffix, vector type and element type.  */
#define TYPES(X) \
  X (, s8, int8x8_t, int8_t) \
  X (q, s8, int8x16_t, int8_t) \
  X (, s16, int16x4_t, int16_t) \
  X (q, s16, int16x8_t, int16_t) \
  X (, s32, int32x2_t, int32_t) \
  X (q, s32, int32x4_t, int32_t) \
  X (q, s64, int64x2_t, int64_t)

#define DEF_ADDV(Q, S, T, E) \
  NOIPA E test_vaddv##Q##_##S (T a) \
  { \
    return vaddv##Q##_##S (a + vdup##Q##_n_##S (1)); \
  }

TYPES (DEF_ADDV)

int
main (void)
{
  /* The first element plus the number of elements exceeds the maximum by
     one, so the sum wraps to the minimum.  */
  if (test_vaddv_s8 ((int8x8_t) { INT8_MAX - 7 }) != INT8_MIN)
    abort ();
  if (test_vaddvq_s8 ((int8x16_t) { INT8_MAX - 15 }) != INT8_MIN)
    abort ();
  if (test_vaddv_s16 ((int16x4_t) { INT16_MAX - 3 }) != INT16_MIN)
    abort ();
  if (test_vaddvq_s16 ((int16x8_t) { INT16_MAX - 7 }) != INT16_MIN)
    abort ();
  if (test_vaddv_s32 ((int32x2_t) { INT32_MAX - 1 }) != INT32_MIN)
    abort ();
  if (test_vaddvq_s32 ((int32x4_t) { INT32_MAX - 3 }) != INT32_MIN)
    abort ();
  if (test_vaddvq_s64 ((int64x2_t) { INT64_MAX - 1 }) != INT64_MIN)
    abort ();

  return 0;
}
