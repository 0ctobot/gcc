/* PR tree-optimization/127335 */
/* { dg-do run } */
/* { dg-require-effective-target int128 } */

typedef unsigned __int128 u128;
typedef unsigned long long u64;

#define U128(hi, lo) (((u128) hi##ULL << 64) | lo##ULL)

/* The expected results are hard-coded since a reference CRC loop here would
   be replaced in the same way as the loops under test.  */

/* A CRC wider than 64 bits with a polynomial above 2^64.  */

__attribute__((noipa)) u128
rev128 (u128 c)
{
  for (int k = 0; k < 8; k++)
    if (c & 1)
      c = U128 (0xedb88320edb88320, 0x1234567890abcdef) ^ (c >> 1);
    else
      c = c >> 1;
  return c;
}

__attribute__((noipa)) u128
fwd128 (u128 c)
{
  for (int k = 0; k < 8; k++)
    if (c >> 127)
      c = (c << 1) ^ U128 (0x04c11db704c11db7, 0x0123456789abcdef);
    else
      c = c << 1;
  return c;
}

/* A polynomial above 2^64 that only exists in the values of the PHIs.  */

__attribute__((noipa)) u128
phi128 (u128 c)
{
  u128 x = 0;
  for (int k = 0; k < 8; k++)
    {
      u128 nx = ~x;
      u128 y = (x >> 1) ^ (nx >> 1);
      if (c & 1)
	c = (c >> 1) ^ y;
      else
	c = c >> 1;
      x = nx;
    }
  return c;
}

/* A CRC that fits in 64 bits with a counter that doesn't.  */

#define CRC8_STEP(c) ((c) & 0x80 ? ((c) << 1) ^ 0x07 : (c) << 1)

__attribute__((noipa)) unsigned char
crc8_big_counter (unsigned char c)
{
  for (u128 i = (u128) 1 << 100; i != ((u128) 1 << 100) + 8; i++)
    c = CRC8_STEP (c);
  return c;
}

__attribute__((noipa)) unsigned char
crc8_neg_counter (unsigned char c)
{
  for (__int128 i = -8; i < 0; i++)
    c = CRC8_STEP (c);
  return c;
}

__attribute__((noipa)) unsigned char
crc8_crossing_counter (unsigned char c)
{
  for (u128 i = ((u128) 1 << 64) - 4; i != ((u128) 1 << 64) + 4; i++)
    c = CRC8_STEP (c);
  return c;
}

__attribute__((noipa)) unsigned char
crc8_signed_crossing_counter (unsigned char c)
{
  for (__int128 i = ((__int128) 1 << 64) - 8; i < ((__int128) 1 << 64); i++)
    c = CRC8_STEP (c);
  return c;
}

__attribute__((noipa)) u64
crc64_big_counter (u64 c)
{
  for (u128 i = (u128) 1 << 70; i != ((u128) 1 << 70) + 8; i++)
    if (c >> 63)
      c = (c << 1) ^ 0x42f0e1eba9ea3693ULL;
    else
      c = c << 1;
  return c;
}

#define CHECK_CRC8(fn)			\
  do					\
    {					\
      if (fn (0x01) != 0x07		\
	  || fn (0x80) != 0x89		\
	  || fn (0xa5) != 0x72		\
	  || fn (0xff) != 0xf3)		\
	__builtin_abort ();		\
    }					\
  while (0)

int
main (void)
{
  u128 a = U128 (0x0123456789abcdef, 0xfedcba9876543211);
  u128 b = U128 (0xdeadbeefcafebabe, 0x0011223344556677);

  if (rev128 (a) != U128 (0xf263230b55ebab83, 0x3fe0b9f809ab18cc)
      || rev128 (b) != U128 (0xb263a6961d77f592, 0xe2e2defca5ae1ffd))
    __builtin_abort ();

  if (fwd128 (a) != U128 (0x27847a3eaf0cf249, 0xdd99dd11dd99dcef)
      || fwd128 (b) != U128 (0x80ab042ad3af55e3, 0xd6d1f4df9295b09b))
    __builtin_abort ();

  if (phi128 (a) != U128 (0x110123456789abcd, 0xeffedcba98765432)
      || phi128 (b) != U128 (0x77deadbeefcafeba, 0xbe00112233445566))
    __builtin_abort ();

  CHECK_CRC8 (crc8_big_counter);
  CHECK_CRC8 (crc8_neg_counter);
  CHECK_CRC8 (crc8_crossing_counter);
  CHECK_CRC8 (crc8_signed_crossing_counter);

  if (crc64_big_counter (0x8000000000000001ULL) != 0xf6fae5c07d3275cdULL
      || crc64_big_counter (0x0123456789abcdefULL) != 0x61b586620227d993ULL)
    __builtin_abort ();

  return 0;
}
