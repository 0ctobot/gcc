/* { dg-require-effective-target vect_int } */
/* { dg-require-effective-target vect_long_long } */

/* Two accesses with the same variable stride don't overlap across iterations
   when the stride is at least the number of bytes they span together.  That
   span is the size of the lower access when it contains the higher one.  */

#include "tree-vect.h"

typedef unsigned long long u64;
typedef unsigned int u32;
typedef unsigned short u16;

#define LOAD(T, OFF, DST) \
  __builtin_memcpy (&DST, p + i * stride + OFF, sizeof (T))
#define STORE(T, OFF, SRC) \
  __builtin_memcpy (p + i * stride + OFF, &SRC, sizeof (T))

#define DEFINE(NAME, BODY)						\
  __attribute__ ((noipa)) u32						\
  NAME (char *p, long stride, int n, u64 v)				\
  {									\
    u32 sum = 0;							\
    for (int i = 0; i < n; ++i)						\
      {									\
	BODY								\
      }									\
    return sum;								\
  }									\
									\
  __attribute__ ((noipa)) u32						\
  NAME##_ref (char *p, long stride, int n, u64 v)			\
  {									\
    u32 sum = 0;							\
    for (int i = 0; i < n; ++i)						\
      {									\
	BODY								\
	__asm__ volatile ("" : : : "memory");				\
      }									\
    return sum;								\
  }

/* Low 8-byte store, 4-byte load inside it and before it.  This goes wrong for
   strides of 5 and 6.  */
DEFINE (load_then_store,
	u32 t;
	LOAD (u32, 1, t);
	sum += t * 3;
	STORE (u64, 0, v);)

/* Low 8-byte store, 4-byte load inside it and after it.  This goes wrong for
   strides of -5 and -6.  */
DEFINE (store_then_load,
	u32 t;
	STORE (u64, 0, v);
	LOAD (u32, 1, t);
	sum += t * 3;)

/* Low 8-byte load, 4-byte store inside it and before it.  This goes wrong for
   strides of 5 and 6.  */
DEFINE (store_then_wide_load,
	u32 w = v;
	u64 t;
	STORE (u32, 1, w);
	LOAD (u64, 0, t);
	sum += t ^ (t >> 32);)

/* Low 8-byte load, 4-byte store inside it and after it.  This goes wrong for
   strides of -5 and -6.  */
DEFINE (wide_load_then_store,
	u32 w = v;
	u64 t;
	LOAD (u64, 0, t);
	sum += t ^ (t >> 32);
	STORE (u32, 1, w);)

/* Two stores with the larger one lower and first.  This goes wrong for strides
   of -5 and -6.  */
DEFINE (wide_then_narrow,
	u32 w = v + 1;
	STORE (u64, 0, v);
	STORE (u32, 1, w);)

/* Two stores at the same offset with the larger one first.  This goes wrong
   for strides from -4 to -7.  */
DEFINE (same_offset,
	u32 w = v + 1;
	STORE (u64, 0, v);
	STORE (u32, 0, w);)

/* A group of two 2-byte loads inside the store.  This goes wrong for strides
   of 5 and 6.  */
DEFINE (load_group,
	u16 t0;
	u16 t1;
	LOAD (u16, 1, t0);
	LOAD (u16, 3, t1);
	sum += t0 + t1 * 3;
	STORE (u64, 0, v);)

static char a[4096], b[4096];

typedef u32 (*fn) (char *, long, int, u64);

static void
check (fn f, fn ref)
{
  for (long stride = -12; stride <= 12; ++stride)
    {
      __builtin_memset (a, 0x5a, sizeof (a));
      __builtin_memset (b, 0x5a, sizeof (b));
      u32 s1 = f (a + 2048, stride, 100, 0x0102030405060708ULL);
      u32 s2 = ref (b + 2048, stride, 100, 0x0102030405060708ULL);
      if (s1 != s2 || __builtin_memcmp (a, b, sizeof (a)) != 0)
	__builtin_abort ();
    }
}

int
main (void)
{
  check_vect ();

  check (load_then_store, load_then_store_ref);
  check (store_then_load, store_then_load_ref);
  check (store_then_wide_load, store_then_wide_load_ref);
  check (wide_load_then_store, wide_load_then_store_ref);
  check (wide_then_narrow, wide_then_narrow_ref);
  check (same_offset, same_offset_ref);
  check (load_group, load_group_ref);
  return 0;
}

/* { dg-final { scan-tree-dump {no alias between [^\n]* when [^\n]* is outside \(-8, 8\)} "vect" { target aarch64*-*-* } } } */
