/* The dependence tests only compare where accesses start, so a store that
   shares just some of its bytes with a load must not leave predictive commoning
   free to reuse the loaded value across it.  */
/* { dg-do run } */
/* { dg-options "-O3" } */
/* { dg-require-effective-target int32plus } */

typedef __UINT64_TYPE__ u64;
typedef __UINT32_TYPE__ u32;
typedef __INT32_TYPE__ s32;
/* The access types are may_alias so that the test doesn't depend on type
   based alias analysis telling the accesses apart.  */
typedef u64 __attribute__ ((aligned (1), may_alias)) u64_1;
typedef u64 __attribute__ ((aligned (4), may_alias)) u64_4;

struct __attribute__ ((may_alias)) pair { u64_4 x, y; };
struct __attribute__ ((may_alias)) flex { s32 n; s32 d[]; };

#define N 64

static unsigned char a[4096] __attribute__ ((aligned (16)));
static unsigned char e[4096] __attribute__ ((aligned (16)));

/* These match the Linux kernel's get_unaligned and put_unaligned.  */
#define GET(T, P)							\
  ({									\
    const struct { T x; } __attribute__ ((packed, may_alias)) *p_	\
      = (const void *) (P);						\
    p_->x;								\
  })
#define PUT(T, P, V)							\
  do									\
    {									\
      struct { T x; } __attribute__ ((packed, may_alias)) *p_		\
	= (void *) (P);							\
      p_->x = (V);							\
    }									\
  while (0)

#define GET_1(T, P) (*(u64_1 *) (P))
#define PUT_1(T, P, V) (*(u64_1 *) (P) = (V))
#define GET_PAIR(T, P) (((struct pair *) (P))->x)
#define PUT_PAIR(T, P, V) (((struct pair *) (P))->y = (V))
#define GET_FLEX(T, P) (((struct flex *) (P))->d[0])
#define PUT_FLEX(T, P, V) (((struct flex *) (P))->d[3] = (V))

/* The expected results come from volatile accesses to individual bytes, which
   nothing can reuse.  */
#define REF_GET(T, P)							\
  ({									\
    T v_;								\
    unsigned char t_[sizeof (T)];					\
    volatile unsigned char *p_ = (P);					\
    for (unsigned int k_ = 0; k_ < sizeof (T); k_++)			\
      t_[k_] = p_[k_];							\
    __builtin_memcpy (&v_, t_, sizeof (T));				\
    v_;									\
  })
#define REF_PUT(T, P, V)						\
  do									\
    {									\
      T v_ = (V);							\
      unsigned char t_[sizeof (T)];					\
      volatile unsigned char *p_ = (P);					\
      __builtin_memcpy (t_, &v_, sizeof (T));				\
      for (unsigned int k_ = 0; k_ < sizeof (T); k_++)			\
	p_[k_] = t_[k_];						\
    }									\
  while (0)
#define REF_PUT_PAIR(T, P, V) REF_PUT (T, (P) + 8, V)
#define REF_GET_FLEX(T, P) REF_GET (T, (P) + 4)
#define REF_PUT_FLEX(T, P, V) REF_PUT (T, (P) + 16, V)

#define LOOP(NAME, T, G, P, STRIDE, L1, L2, ST)				\
  __attribute__ ((noipa)) static u64					\
  NAME (unsigned char *q)						\
  {									\
    u64 s = 0;								\
    for (long i = 0; i < N; i++)					\
      {									\
	s += G (T, q + (STRIDE) * i + (L1));				\
	s += G (T, q + (STRIDE) * i + (L2));				\
	P (T, q + (STRIDE) * i + (ST), i);				\
      }									\
    return s;								\
  }

/* The store in iteration I covers the last 4 bytes of the second load in
   iteration I - 1, and the first load in iteration I + 1 covers the end of
   it.  */
LOOP (una8, u64, GET, PUT, 20, 0, 40, 16)
LOOP (una8_ref, u64, REF_GET, REF_PUT, 20, 0, 40, 16)
LOOP (una4, u32, GET, PUT, 6, 0, 12, 4)
LOOP (una4_ref, u32, REF_GET, REF_PUT, 6, 0, 12, 4)
LOOP (down, u64, GET, PUT, -20, 0, -40, -16)
LOOP (down_ref, u64, REF_GET, REF_PUT, -20, 0, -40, -16)
LOOP (align1, u64, GET_1, PUT_1, 20, 0, 40, 16)
LOOP (align1_ref, u64, REF_GET, REF_PUT, 20, 0, 40, 16)
/* The stride is smaller than the structure, so the Y of one iteration overlaps
   the X of the next.  */
LOOP (pair, u64, GET_PAIR, PUT_PAIR, 12, 0, 48, 0)
LOOP (pair_ref, u64, REF_GET, REF_PUT_PAIR, 12, 0, 48, 0)
/* The stride is a multiple of the structure's size, but the flexible array
   reaches into the structure of the next iteration.  */
LOOP (flex, s32, GET_FLEX, PUT_FLEX, 12, 0, 24, 0)
LOOP (flex_ref, s32, REF_GET_FLEX, REF_PUT_FLEX, 12, 0, 24, 0)

#define RUN(NAME)							\
  do									\
    {									\
      for (int i = 0; i < 4096; i++)					\
	a[i] = e[i] = i * 37 + 11 + (i >> 3);				\
      if (NAME (a + 2048) != NAME##_ref (e + 2048)			\
	  || __builtin_memcmp (a, e, sizeof (a)))			\
	__builtin_abort ();						\
    }									\
  while (0)

int
main (void)
{
  RUN (una8);
  RUN (una4);
  RUN (down);
  RUN (align1);
  RUN (pair);
  RUN (flex);
  return 0;
}
