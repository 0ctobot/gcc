/* Bit-serial loops that sign-extend their register into a wider temporary
   and then shift the temporary right bring copies of the sign bit into the
   register, so they aren't CRCs.  Each must compute the same values as its
   ref_ copy, whose iteration count the CRC pass can't see.  Genuine CRC
   loops on signed registers, which zero-extend the register or shift it
   left, must still compute the right values.  */
/* { dg-do run } */
/* { dg-require-effective-target int32plus } */

#define NI __attribute__ ((noipa))

typedef signed char s8;
typedef unsigned char u8;

volatile int n8 = 8, n32 = 32;

/* A signed char register read into an int temporary.  */
#define B_S8_INT(N)					\
  crc ^= data;						\
  for (int i = 0; i < (N); i++)				\
    {							\
      int t = crc;					\
      if (t & 1)					\
	t = (t >> 1) ^ 0x8C;				\
      else						\
	t >>= 1;					\
      crc = (s8) t;					\
    }							\
  return crc;

NI s8 s8_int (s8 crc, u8 data) { B_S8_INT (8) }
NI s8 ref_s8_int (s8 crc, u8 data) { B_S8_INT (n8) }

/* An unsigned char register read through a signed char cast.  */
#define B_U8_CAST(N)					\
  crc ^= data;						\
  for (int i = 0; i < (N); i++)				\
    {							\
      int t = (s8) crc;					\
      if (t & 1)					\
	t = (t >> 1) ^ 0x8C;				\
      else						\
	t >>= 1;					\
      crc = t;						\
    }							\
  return crc;

NI u8 u8_cast (u8 crc, u8 data) { B_U8_CAST (8) }
NI u8 ref_u8_cast (u8 crc, u8 data) { B_U8_CAST (n8) }

/* A short register read into an int temporary.  */
#define B_S16_INT(N)					\
  crc ^= data;						\
  for (int i = 0; i < (N); i++)				\
    {							\
      int x = crc;					\
      if (x & 1)					\
	x = (x >> 1) ^ 0xA001;				\
      else						\
	x >>= 1;					\
      crc = x;						\
    }							\
  return crc;

NI short s16_int (short crc, u8 data) { B_S16_INT (8) }
NI short ref_s16_int (short crc, u8 data) { B_S16_INT (n8) }

/* An int register read into a long long temporary, 8 iterations.  */
#define B_S32_LL(N)					\
  crc ^= data;						\
  for (int i = 0; i < (N); i++)				\
    {							\
      long long t = crc;				\
      if (t & 1)					\
	crc = (t >> 1) ^ 0xEDB88320;			\
      else						\
	crc = t >> 1;					\
    }							\
  return crc;

NI int s32_ll (int crc, u8 data) { B_S32_LL (8) }
NI int ref_s32_ll (int crc, u8 data) { B_S32_LL (n8) }

/* The same with 32 iterations and 32-bit data.  */
#define B_S32_LL32(N)					\
  crc ^= data;						\
  for (int i = 0; i < (N); i++)				\
    {							\
      long long x = crc;				\
      crc = x >> 1;					\
      if (x & 1)					\
	crc ^= 0xEDB88320;				\
    }							\
  return crc;

NI int s32_ll32 (int crc, unsigned int data) { B_S32_LL32 (32) }
NI int ref_s32_ll32 (int crc, unsigned int data) { B_S32_LL32 (n32) }

/* A signed char register converted to unsigned short for the shift.  */
#define B_S8_USHORT(N)					\
  for (int i = 0; i < (N); i++)				\
    if (crc & 1)					\
      crc = ((unsigned short) crc >> 1) ^ (s8) 0x8C;	\
    else						\
      crc = (unsigned short) crc >> 1;			\
  return crc;

NI s8 s8_ushort (s8 crc) { B_S8_USHORT (8) }
NI s8 ref_s8_ushort (s8 crc) { B_S8_USHORT (n8) }

/* A short register shifted through an int temporary before the xor.  */
#define B_S16_SHIFT_FIRST(N)				\
  crc ^= data;						\
  for (int i = 0; i < (N); i++)				\
    {							\
      int t = crc;					\
      crc = t >> 1;					\
      if (t & 1)					\
	crc ^= 0xA001u;					\
    }							\
  return crc;

NI short s16_shift_first (short crc, u8 data) { B_S16_SHIFT_FIRST (8) }
NI short ref_s16_shift_first (short crc, u8 data) { B_S16_SHIFT_FIRST (n8) }

/* A signed char register whose int temporary is xored with the data
   inside the loop.  */
#define B_S8_DATA_IN_LOOP(N)				\
  for (int i = 0; i < (N); i++)				\
    {							\
      int t = crc;					\
      if ((t ^ data) & 1)				\
	t = (t >> 1) ^ 0x8C;				\
      else						\
	t >>= 1;					\
      crc = t;						\
      data >>= 1;					\
    }							\
  return crc;

NI s8 s8_data_in_loop (s8 crc, s8 data) { B_S8_DATA_IN_LOOP (8) }
NI s8 ref_s8_data_in_loop (s8 crc, s8 data) { B_S8_DATA_IN_LOOP (n8) }

/* A genuine forward CRC-8 on a signed char register: the bits that the
   sign extension adds are shifted away from the register's bits.  */
#define B_OK_S8_FWD(N)					\
  crc ^= data;						\
  for (int i = 0; i < (N); i++)				\
    {							\
      int t = crc;					\
      if (t & 0x80)					\
	t = (t << 1) ^ 0x07;				\
      else						\
	t <<= 1;					\
      crc = (s8) t;					\
    }							\
  return crc;

NI s8 ok_s8_fwd (s8 crc, u8 data) { B_OK_S8_FWD (8) }
NI s8 ref_ok_s8_fwd (s8 crc, u8 data) { B_OK_S8_FWD (n8) }

/* A genuine reflected CRC-16 on a short register that is zero-extended.  */
#define B_OK_S16_ZEXT(N)				\
  crc ^= data;						\
  for (int i = 0; i < (N); i++)				\
    {							\
      int t = (unsigned short) crc;			\
      if (t & 1)					\
	t = (t >> 1) ^ 0xA001;				\
      else						\
	t >>= 1;					\
      crc = (short) t;					\
    }							\
  return crc;

NI short ok_s16_zext (short crc, u8 data) { B_OK_S16_ZEXT (8) }
NI short ref_ok_s16_zext (short crc, u8 data) { B_OK_S16_ZEXT (n8) }

/* A genuine reflected CRC-32 on an int register that is zero-extended.  */
#define B_OK_S32_ZEXT(N)				\
  crc ^= data;						\
  for (int i = 0; i < (N); i++)				\
    {							\
      unsigned long long t = (unsigned int) crc;	\
      if (t & 1)					\
	crc = (t >> 1) ^ 0xEDB88320;			\
      else						\
	crc = t >> 1;					\
    }							\
  return crc;

NI int ok_s32_zext (int crc, u8 data) { B_OK_S32_ZEXT (8) }
NI int ref_ok_s32_zext (int crc, u8 data) { B_OK_S32_ZEXT (n8) }

static unsigned int seed = 12345;

static unsigned int
rnd (void)
{
  seed = seed * 1103515245u + 12345u;
  return (seed >> 16) ^ (seed << 16);
}

int
main (void)
{
  for (int c = 0; c < 256; c++)
    for (int d = 0; d < 256; d++)
      {
	if (s8_int (c, d) != ref_s8_int (c, d)
	    || u8_cast (c, d) != ref_u8_cast (c, d)
	    || s8_ushort (c) != ref_s8_ushort (c)
	    || s8_data_in_loop (c, d) != ref_s8_data_in_loop (c, d)
	    || ok_s8_fwd (c, d) != ref_ok_s8_fwd (c, d))
	  __builtin_abort ();
	short c16 = c * 257 + d;
	if (s16_int (c16, d) != ref_s16_int (c16, d)
	    || s16_shift_first (c16, d) != ref_s16_shift_first (c16, d)
	    || ok_s16_zext (c16, d) != ref_ok_s16_zext (c16, d))
	  __builtin_abort ();
      }

  for (int k = 0; k < 20000; k++)
    {
      int c = rnd ();
      unsigned int d = rnd ();
      if (s32_ll (c, d) != ref_s32_ll (c, d)
	  || s32_ll32 (c, d) != ref_s32_ll32 (c, d)
	  || ok_s32_zext (c, d) != ref_ok_s32_zext (c, d))
	__builtin_abort ();
    }

  /* The arithmetic shift fills the register with ones here, unlike a
     reflected CRC-8 with polynomial 0x31, which returns 0x8c.  */
  if ((u8) s8_int ((s8) 0x80, 0) != 0x73)
    __builtin_abort ();

  return 0;
}
