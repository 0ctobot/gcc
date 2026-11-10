/* Check that __builtin_return_address (0) returns the exact, unsigned
   return address when return address signing is enabled, whether it's read
   on a cold path that shrink-wrapping separates from the prologue or on the
   hot path.  */
/* { dg-do run } */
/* { dg-options "-O2 -mbranch-protection=pac-ret" } */

#define NOIPA __attribute__ ((noipa, used))

extern void abort (void);

void *volatile sink;

NOIPA void
take (void *p)
{
  sink = p;
}

/* Read only on a cold path that makes a call.  */
NOIPA void *
cold_call (long c)
{
  if (__builtin_expect (c != 0, 0))
    {
      take (__builtin_return_address (0));
      return sink;
    }
  return 0;
}

/* Read only on a cold path that doesn't make a call.  */
NOIPA void *
cold_leaf (long c)
{
  if (__builtin_expect (c != 0, 0))
    return __builtin_return_address (0);
  return 0;
}

/* Read on the hot path after a call.  */
NOIPA void *
hot_call (long c)
{
  take (0);
  return __builtin_return_address (0);
}

/* Read in a leaf function.  */
NOIPA void *
leaf (long c)
{
  return __builtin_return_address (0);
}

/* Read after a variable-sized allocation, which needs a frame pointer.  */
NOIPA void *
dynamic (long c)
{
  char *p = __builtin_alloca (c + 16);
  p[0] = 1;
  take (p);
  return __builtin_return_address (0);
}

/* Read with a large local array.  */
NOIPA void *
big_frame (long c)
{
  char buf[70000];
  buf[c] = 1;
  take (buf);
  if (__builtin_expect (c != 0, 0))
    {
      take (__builtin_return_address (0));
      return sink;
    }
  return 0;
}

/* Read on both paths.  */
NOIPA void *
both (long c)
{
  if (__builtin_expect (c > 1, 0))
    {
      take (__builtin_return_address (0));
      return sink;
    }
  return __builtin_return_address (0);
}

/* Read twice on the cold path, with values live across a call.  */
NOIPA void *
twice (long c)
{
  if (__builtin_expect (c != 0, 0))
    {
      void *a = __builtin_return_address (0);
      take (a);
      void *b = __builtin_return_address (0);
      if (a != b || sink != a)
	abort ();
      return b;
    }
  return 0;
}

/* Signing disabled for this function only.  */
__attribute__ ((target ("branch-protection=none"))) NOIPA void *
unsigned_ra (long c)
{
  if (__builtin_expect (c != 0, 0))
    {
      take (__builtin_return_address (0));
      return sink;
    }
  return 0;
}

/* Signing with the B key for this function only.  */
__attribute__ ((target ("branch-protection=pac-ret+b-key"))) NOIPA void *
b_key (long c)
{
  if (__builtin_expect (c != 0, 0))
    {
      take (__builtin_return_address (0));
      return sink;
    }
  return 0;
}

/* Signing of leaf functions enabled for this function only.  */
__attribute__ ((target ("branch-protection=pac-ret+leaf"))) NOIPA void *
leaf_signed (long c)
{
  if (__builtin_expect (c != 0, 0))
    return __builtin_return_address (0);
  return 0;
}

#ifdef __ARM_FEATURE_SVE
#include <arm_sve.h>

/* Read with a scalable local, which makes the offset of the return address
   slot from the stack pointer depend on the vector length.  */
NOIPA void *
scalable (long c)
{
  svint32_t v = svdup_s32 (c);
  take (&v);
  if (__builtin_expect (c != 0, 0))
    {
      take (__builtin_return_address (0));
      return sink;
    }
  return 0;
}

#define SVE_CLOBBERS							\
  , "p0", "p1", "p2", "p3", "p4", "p5", "p6", "p7", "p8", "p9", "p10",	\
  "p11", "p12", "p13", "p14", "p15"
#else
#define SVE_CLOBBERS
#endif

#ifdef __ARM_NEON
#define FP_CLOBBERS							\
  , "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v8", "v9", "v10",	\
  "v11", "v12", "v13", "v14", "v15", "v16", "v17", "v18", "v19", "v20",	\
  "v21", "v22", "v23", "v24", "v25", "v26", "v27", "v28", "v29", "v30",	\
  "v31"
#else
#define FP_CLOBBERS
#endif

/* Call FN with ARG and check that it returns the address right after the
   call.  */
#define CHECK(FN, ARG)						\
  do									\
    {									\
      register long x0_ __asm__ ("x0") = (ARG);				\
      void *ra_;							\
      __asm__ volatile ("bl\t" #FN "\n\tadr\t%1, ."			\
			: "+r" (x0_), "=r" (ra_)			\
			:						\
			: "x1", "x2", "x3", "x4", "x5", "x6", "x7", "x8", \
			  "x9", "x10", "x11", "x12", "x13", "x14", "x15", \
			  "x16", "x17", "x18", "x30", "cc", "memory"	\
			  FP_CLOBBERS SVE_CLOBBERS);					\
      if ((void *) x0_ != ra_)						\
	abort ();							\
    }									\
  while (0)

int
main (void)
{
  if (cold_call (0) || cold_leaf (0))
    abort ();
  for (int i = 0; i < 2; i++)
    {
      CHECK (cold_call, 1);
      CHECK (cold_leaf, 1);
      CHECK (hot_call, 0);
      CHECK (leaf, 0);
      CHECK (dynamic, 100);
      CHECK (big_frame, 1);
      CHECK (both, 0);
      CHECK (both, 2);
      CHECK (twice, 1);
      CHECK (unsigned_ra, 1);
      CHECK (b_key, 1);
      CHECK (leaf_signed, 1);
#ifdef __ARM_FEATURE_SVE
      CHECK (scalable, 1);
#endif
    }
  return 0;
}
