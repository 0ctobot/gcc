/* Every constant-offset read of a sparsely initialized read-only array must
   match the same read through a pointer the compiler can't see through.  */
/* { dg-do run } */

struct pad { int a; char b; };

static const int a1[8] = { [3] = 0x11223344, [5] = 0x55667788 };
static const struct pad a2[4] = { [1] = { 0x01020304, 0x7f },
				  [3] = { 0x0a0b0c0d, 0x11 } };
static const int a3[3][2] = { [1] = { [1] = 0xdeadbeef },
			      [2] = { 0xcafef00d } };
static const short a4[8] = { [2] = 0x1234, [5] = 0x5678, [7] = 0x7abc };
static const long a5[4] = { [1] = 0x11223344, [3] = -2 };
static const struct { short s[3]; char c[2]; } a6[3] = {
  [0] = { .s = { [2] = 0x4142 }, .c = { [1] = 0x43 } },
  [2] = { .s = { 0x5152 }, .c = { 0x53 } },
};
static const unsigned char a7[16] = { [3] = 0x81, [4] = 0x82, [9] = 0x83,
				      [15] = 0x84 };

static int wrong;

#define CHK(arr, T, off)						\
  do									\
    {									\
      if ((off) + sizeof (T) <= sizeof (arr))				\
	{								\
	  const char *p_ = (const char *) (arr);			\
	  T f_, r_;							\
	  __builtin_memcpy (&f_, (const char *) (arr) + (off),		\
			    sizeof (T));				\
	  __asm__ ("" : "+r" (p_));					\
	  __builtin_memcpy (&r_, p_ + (off), sizeof (T));		\
	  if (f_ != r_)							\
	    wrong++;							\
	}								\
    }									\
  while (0);

#define O4(arr, T, b) CHK (arr, T, b) CHK (arr, T, (b) + 1) \
  CHK (arr, T, (b) + 2) CHK (arr, T, (b) + 3)
#define O16(arr, T, b) O4 (arr, T, b) O4 (arr, T, (b) + 4) \
  O4 (arr, T, (b) + 8) O4 (arr, T, (b) + 12)
#define O32(arr, T) O16 (arr, T, 0) O16 (arr, T, 16)
#define ALLT(arr) O32 (arr, unsigned char) O32 (arr, unsigned short) \
  O32 (arr, unsigned int) O32 (arr, unsigned long long)

__attribute__((noipa)) void t1 (void) { ALLT (a1) }
__attribute__((noipa)) void t2 (void) { ALLT (a2) }
__attribute__((noipa)) void t3 (void) { ALLT (a3) }
__attribute__((noipa)) void t4 (void) { ALLT (a4) }
__attribute__((noipa)) void t5 (void) { ALLT (a5) }
__attribute__((noipa)) void t6 (void) { ALLT (a6) }
__attribute__((noipa)) void t7 (void) { ALLT (a7) }

int
main (void)
{
  t1 ();
  t2 ();
  t3 ();
  t4 ();
  t5 ();
  t6 ();
  t7 ();
  if (wrong)
    __builtin_abort ();
  return 0;
}
