/* A constant-offset read of an initialized array that starts in one element
   and runs into the next must not fold to zero.  Each read is compared with
   the same read through a pointer the compiler can't see through.  */
/* { dg-do run } */
/* { dg-require-effective-target int128 } */

typedef unsigned long long u64;
typedef unsigned int u32;
typedef unsigned short u16;

static int wrong;

#define CHECK(obj, T, off)					\
  do								\
    {								\
      const char *p_ = (const char *) &(obj);			\
      T f_, r_;							\
      __builtin_memcpy (&f_, (const char *) &(obj) + (off),	\
			sizeof (T));				\
      __asm__ ("" : "+r" (p_));					\
      __builtin_memcpy (&r_, p_ + (off), sizeof (T));		\
      if (f_ != r_)						\
	wrong++;						\
    }								\
  while (0)

/* The first element the read touches isn't initialized.  */
static const __int128 ga[2] = { [1] = 0x80000000ULL };
static const u32 t32[4] = { [1] = 0x11223344, [3] = 5 };
struct hdr { u16 tag; u32 f[3]; };
static const struct hdr H = { .tag = 7, .f[1] = 0xaabbccdd };
/* The read starts in the padding of an initialized element.  */
struct P { u32 a; u16 b; };
static const struct P arr[4] = { [2] = { 1, 2 }, [3] = { .a = 0x11223344 } };

/* Packed and underaligned members.  */
typedef long s64_a1 __attribute__((aligned (1)));
typedef unsigned int u32_a1 __attribute__((aligned (1)));
enum E { E0, E1, E2, E3 };
struct s0
{
  s64_a1 m1[5];
  unsigned short m2 : 12;
  int m3;
  u32_a1 m4;
} __attribute__((packed));
static const struct s0 a = { .m4 = 0xffffffff, .m1[2] = 0x61a0611a7251438LL,
			     .m1[1] = 0xa };
struct s9
{
  enum E m31[7];
  long long m32 __attribute__((packed, aligned (1)));
  unsigned long m33 : 32;
} __attribute__((packed));
static const struct s9 b = { .m33 = 0xffffffff, .m31[4] = E3 };
struct s3 { unsigned int m5[3]; };
static const struct s3 c = { .m5[2] = 0x94541a2d, .m5[1] = 0xffffffff };

__attribute__((noipa)) void
test (void)
{
  CHECK (ga, u64, 15);
  CHECK (t32, u16, 3);
  CHECK (H, u32, 6);
  CHECK (arr, u32, 22);
  CHECK (a, u64, 7);
  CHECK (b, u32, 14);
  CHECK (c, u32, 1);
}

int
main (void)
{
  test ();
  if (wrong)
    __builtin_abort ();
  return 0;
}
