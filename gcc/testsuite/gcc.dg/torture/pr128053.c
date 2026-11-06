/* { dg-do run } */
/* { dg-require-effective-target int32plus } */
/* { dg-additional-options "-ftree-vectorize" } */

/* Reading a union member after storing to another member leaves a
   BIT_FIELD_REF of a signed value.  An unsigned field that includes the sign
   bit must still be zero-extended when the read is vectorized.  Which field
   holds the sign bit depends on the endianness, so check both of them.  */

#define N 32

union u32
{
  int i;
  struct { unsigned f1 : 24; unsigned f2 : 8; } s;
};

union u16
{
  short i;
  struct { unsigned short f1 : 8; unsigned short f2 : 8; } s;
};

union u64
{
  long long i;
  struct { unsigned long long f1 : 40; unsigned long long f2 : 24; } s;
};

union bytes
{
  int i;
  unsigned char b[4];
};

int u32_in[N];
short u16_in[N];
long long u64_in[N];
int bytes_in[N];

/* TY is the result type and WANT is the zero-extended value of an all-ones
   field.  The conversion from the container to TY and the addition as the
   only use of the field are what make the vectorizer mask before it shifts,
   so cover both of them, plus a reduction.  */
#define TEST(NAME, UNION, FIELD, TY, WANT)				\
  TY NAME##_out[N];							\
  TY NAME##_add[N];							\
									\
  __attribute__ ((noipa)) void						\
  NAME##_conv (void)							\
  {									\
    for (int k = 0; k < N; k++)						\
      {									\
	union UNION u;							\
	u.i = UNION##_in[k];						\
	NAME##_out[k] = u.FIELD;					\
      }									\
  }									\
									\
  __attribute__ ((noipa)) void						\
  NAME##_plus (void)							\
  {									\
    for (int k = 0; k < N; k++)						\
      {									\
	union UNION u;							\
	u.i = UNION##_in[k];						\
	NAME##_out[k] = (TY) u.FIELD + NAME##_add[k];			\
      }									\
  }									\
									\
  __attribute__ ((noipa)) TY						\
  NAME##_sum (void)							\
  {									\
    TY s = 0;								\
    for (int k = 0; k < N; k++)						\
      {									\
	union UNION u;							\
	u.i = UNION##_in[k];						\
	s += u.FIELD;							\
      }									\
    return s;								\
  }									\
									\
  __attribute__ ((noipa)) void						\
  NAME##_check (void)							\
  {									\
    NAME##_conv ();							\
    _Pragma ("GCC novector")						\
    for (int k = 0; k < N; k++)						\
      if (NAME##_out[k] != (TY) (k & 1 ? (WANT) : 0))			\
	__builtin_abort ();						\
									\
    NAME##_plus ();							\
    _Pragma ("GCC novector")						\
    for (int k = 0; k < N; k++)						\
      if (NAME##_out[k] != (TY) (k & 1 ? (WANT) + 1 : 1))		\
	__builtin_abort ();						\
									\
    if (NAME##_sum () != (TY) ((TY) (WANT) * (N / 2)))			\
      __builtin_abort ();						\
  }

TEST (a1, u32, s.f1, unsigned, 0xffffff)
TEST (a2, u32, s.f2, unsigned, 0xff)
TEST (a3, u32, s.f2, unsigned short, 0xff)
TEST (a4, u32, s.f2, short, 0xff)
TEST (a5, u32, s.f1, int, 0xffffff)
TEST (a6, u32, s.f2, int, 0xff)
TEST (b1, u16, s.f1, unsigned short, 0xff)
TEST (b2, u16, s.f2, unsigned short, 0xff)
TEST (b3, u16, s.f1, short, 0xff)
TEST (b4, u16, s.f2, short, 0xff)
TEST (c1, u64, s.f1, unsigned long long, 0xffffffffffull)
TEST (c2, u64, s.f2, unsigned long long, 0xffffffull)
TEST (c3, u64, s.f1, long long, 0xffffffffffll)
TEST (c4, u64, s.f2, long long, 0xffffffll)
TEST (c5, u64, s.f2, unsigned, 0xffffffu)
TEST (c6, u64, s.f2, int, 0xffffff)
TEST (d1, bytes, b[0], unsigned, 0xff)
TEST (d2, bytes, b[3], unsigned, 0xff)
TEST (d3, bytes, b[0], int, 0xff)
TEST (d4, bytes, b[3], int, 0xff)

int
main (void)
{
  _Pragma ("GCC novector")
  for (int k = 0; k < N; k++)
    {
      u32_in[k] = k & 1 ? -1 : 0;
      u16_in[k] = k & 1 ? -1 : 0;
      u64_in[k] = k & 1 ? -1 : 0;
      bytes_in[k] = k & 1 ? -1 : 0;
      a1_add[k] = a2_add[k] = a3_add[k] = a4_add[k] = a5_add[k] = a6_add[k] = 1;
      b1_add[k] = b2_add[k] = b3_add[k] = b4_add[k] = 1;
      c1_add[k] = c2_add[k] = c3_add[k] = c4_add[k] = c5_add[k] = c6_add[k] = 1;
      d1_add[k] = d2_add[k] = d3_add[k] = d4_add[k] = 1;
    }

  a1_check ();
  a2_check ();
  a3_check ();
  a4_check ();
  a5_check ();
  a6_check ();
  b1_check ();
  b2_check ();
  b3_check ();
  b4_check ();
  c1_check ();
  c2_check ();
  c3_check ();
  c4_check ();
  c5_check ();
  c6_check ();
  d1_check ();
  d2_check ();
  d3_check ();
  d4_check ();

  return 0;
}
