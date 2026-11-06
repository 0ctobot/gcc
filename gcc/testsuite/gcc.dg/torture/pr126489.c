/* { dg-do run } */
/* { dg-require-effective-target int32plus } */
/* { dg-additional-options "-Wno-psabi" } */

/* Check the SWAR sign mask ((x >> (L - 1)) & m) * ((1 << L) - 1) on each
   packed lane width L, element type and vector size.  Lanes narrower than a
   byte can't be turned into a vector comparison.  */

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

#define U1(x) x
#define U2(x) x, x
#define U4(x) U2 (x), U2 (x)
#define U8(x) U4 (x), U4 (x)
#define U16(x) U8 (x), U8 (x)

__attribute__ ((noipa)) static u64
ref (u64 x, unsigned bits, unsigned lane)
{
  u64 r = 0;

  for (unsigned k = 0; k < bits; k += lane)
    if ((x >> (k + lane - 1)) & 1)
      r |= ((1ULL << lane) - 1) << k;
  return r;
}

#define F(NAME, ELT, BYTES, NELTS, SH, AND, MUL)			\
  typedef ELT NAME##_t __attribute__ ((vector_size (BYTES)));		\
  __attribute__ ((noipa)) NAME##_t					\
  NAME (NAME##_t x)							\
  {									\
    NAME##_t m = { U##NELTS (AND) };					\
    NAME##_t n = { U##NELTS (MUL) };					\
    return ((x >> SH) & m) * n;						\
  }									\
  static void								\
  NAME##_check (const u64 *in)						\
  {									\
    NAME##_t x, r;							\
    for (unsigned i = 0; i < NELTS; i++)				\
      x[i] = (ELT) in[i];						\
    r = NAME (x);							\
    for (unsigned i = 0; i < NELTS; i++)				\
      if ((u64) r[i] != ref ((ELT) in[i], sizeof (ELT) * 8, SH + 1))	\
	__builtin_abort ();						\
  }

F (q4_2, u8, 4, 4, 1, 0x55, 3)
F (q4_4, u8, 4, 4, 3, 0x11, 15)
F (q4_8, u8, 4, 4, 7, 1, 255)
F (h2_2, u16, 4, 2, 1, 0x5555, 3)
F (h2_4, u16, 4, 2, 3, 0x1111, 15)
F (h2_8, u16, 4, 2, 7, 0x0101, 255)
F (h2_16, u16, 4, 2, 15, 1, 0xffff)
F (s1_2, u32, 4, 1, 1, 0x55555555, 3)
F (s1_4, u32, 4, 1, 3, 0x11111111, 15)
F (s1_8, u32, 4, 1, 7, 0x01010101, 255)
F (s1_16, u32, 4, 1, 15, 0x00010001, 0xffff)

F (q8_2, u8, 8, 8, 1, 0x55, 3)
F (q8_4, u8, 8, 8, 3, 0x11, 15)
F (q8_8, u8, 8, 8, 7, 1, 255)
F (h4_2, u16, 8, 4, 1, 0x5555, 3)
F (h4_4, u16, 8, 4, 3, 0x1111, 15)
F (h4_8, u16, 8, 4, 7, 0x0101, 255)
F (h4_16, u16, 8, 4, 15, 1, 0xffff)
F (s2_2, u32, 8, 2, 1, 0x55555555, 3)
F (s2_4, u32, 8, 2, 3, 0x11111111, 15)
F (s2_8, u32, 8, 2, 7, 0x01010101, 255)
F (s2_16, u32, 8, 2, 15, 0x00010001, 0xffff)
F (s2_32, u32, 8, 2, 31, 1, 0xffffffffu)
F (d1_2, u64, 8, 1, 1, 0x5555555555555555ull, 3)
F (d1_4, u64, 8, 1, 3, 0x1111111111111111ull, 15)
F (d1_8, u64, 8, 1, 7, 0x0101010101010101ull, 255)
F (d1_16, u64, 8, 1, 15, 0x0001000100010001ull, 0xffff)
F (d1_32, u64, 8, 1, 31, 0x0000000100000001ull, 0xffffffffu)

F (q16_4, u8, 16, 16, 3, 0x11, 15)
F (q16_8, u8, 16, 16, 7, 1, 255)
F (h8_4, u16, 16, 8, 3, 0x1111, 15)
F (h8_8, u16, 16, 8, 7, 0x0101, 255)
F (h8_16, u16, 16, 8, 15, 1, 0xffff)
F (s4_4, u32, 16, 4, 3, 0x11111111, 15)
F (s4_8, u32, 16, 4, 7, 0x01010101, 255)
F (s4_16, u32, 16, 4, 15, 0x00010001, 0xffff)
F (s4_32, u32, 16, 4, 31, 1, 0xffffffffu)
F (d2_4, u64, 16, 2, 3, 0x1111111111111111ull, 15)
F (d2_8, u64, 16, 2, 7, 0x0101010101010101ull, 255)
F (d2_16, u64, 16, 2, 15, 0x0001000100010001ull, 0xffff)
F (d2_32, u64, 16, 2, 31, 0x0000000100000001ull, 0xffffffffu)

/* The vectorizer reaches the same lanes from scalar code.  */

__attribute__ ((noipa)) void
loop_u8 (u8 *restrict a)
{
  for (int i = 0; i < 8; i++)
    a[i] = ((a[i] >> 3) & 0x11) * 15;
}

__attribute__ ((noipa)) void
loop_u16 (u16 *restrict a)
{
  for (int i = 0; i < 4; i++)
    a[i] = ((a[i] >> 3) & 0x1111) * 15;
}

static volatile u64 seed[16]
  = { 0x0000000000000000ull, 0x0123456789abcdefull, 0x8888888888888888ull,
      0x1111111111111111ull, 0xffffffffffffffffull, 0x7f7f7f7f7f7f7f7full,
      0x8000800080008000ull, 0x8000000080000000ull, 0x5555555555555555ull,
      0xaaaaaaaaaaaaaaaaull, 0xf00ff00ff00ff00full, 0x18818118a55a3cc3ull,
      0xdeadbeefcafef00dull, 0x0f0f0f0ff0f0f0f0ull, 0x9e3779b97f4a7c15ull,
      0x7000000700000070ull };

int
main (void)
{
  for (unsigned k = 0; k < 16; k++)
    {
      u64 in[16];
      u8 b[8];
      u16 h[4];

      for (unsigned i = 0; i < 16; i++)
	in[i] = seed[(k + i) % 16];

#define C(NAME) NAME##_check (in);
      C (q4_2) C (q4_4) C (q4_8) C (h2_2) C (h2_4) C (h2_8) C (h2_16)
      C (s1_2) C (s1_4) C (s1_8) C (s1_16)
      C (q8_2) C (q8_4) C (q8_8) C (h4_2) C (h4_4) C (h4_8) C (h4_16)
      C (s2_2) C (s2_4) C (s2_8) C (s2_16) C (s2_32)
      C (d1_2) C (d1_4) C (d1_8) C (d1_16) C (d1_32)
      C (q16_4) C (q16_8) C (h8_4) C (h8_8) C (h8_16)
      C (s4_4) C (s4_8) C (s4_16) C (s4_32)
      C (d2_4) C (d2_8) C (d2_16) C (d2_32)

      for (unsigned i = 0; i < 8; i++)
	b[i] = (u8) in[i];
      loop_u8 (b);
      for (unsigned i = 0; i < 8; i++)
	if (b[i] != ref ((u8) in[i], 8, 4))
	  __builtin_abort ();

      for (unsigned i = 0; i < 4; i++)
	h[i] = (u16) in[i];
      loop_u16 (h);
      for (unsigned i = 0; i < 4; i++)
	if (h[i] != ref ((u16) in[i], 16, 4))
	  __builtin_abort ();
    }
  return 0;
}
