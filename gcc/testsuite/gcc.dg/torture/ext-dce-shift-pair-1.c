/* ext-dce used to replace the right shift in x = x << C; t = x >> C with
   t = x, which reads x after the left shift overwrote it and so loses the
   low bits that t needs.  The multiply by 2^C expands to the left shift,
   and the two SSA names of x share a pseudo since they meet in a PHI.  */
/* { dg-do run } */

typedef unsigned long long u64;
typedef long long s64;
typedef unsigned char u8;
typedef unsigned short u16;

#define Q56 (1ULL << 56)
#define Q48 (1ULL << 48)

__attribute__ ((noipa)) s64
sext8 (s64 x, int c, u8 *r)
{
  if (c)
    {
      x *= 1LL << 56;
      *r = x >> 56;
    }
  return x;
}

__attribute__ ((noipa)) u64
zext16 (u64 x, int c, u16 *r)
{
  if (c)
    {
      x *= Q48;
      *r = x / Q48;
    }
  return x;
}

__attribute__ ((noipa)) u64
scale (u64 x, int c, u8 *whole)
{
  if (c)
    {
      x *= Q56;
      *whole = x / Q56;
    }
  return x;
}

__attribute__ ((noipa)) u8
loop (u64 x, const u8 *a, int n)
{
  u8 r = 0;

  for (int i = 0; i < n; i++)
    {
      x *= Q56;
      r ^= (u8) (x >> 56) ^ a[i];
      x |= a[i];
    }
  return r;
}

int
main (void)
{
  u8 b = 0x5a, w = 0x5a, a[1] = { 0 };
  u16 h = 0x5a5a;

  /* -0x78 keeps the signed multiply in range and has 0x88 as its low
     byte.  */
  if ((u64) sext8 (-0x78, 1, &b) != 0x8800000000000000ULL || b != 0x88)
    __builtin_abort ();
  if (zext16 (0x1234567855667788ULL, 1, &h) != 0x7788000000000000ULL
      || h != 0x7788)
    __builtin_abort ();
  if (scale (0x1234567855667788ULL, 1, &w) != 0x8800000000000000ULL
      || w != 0x88)
    __builtin_abort ();
  if (loop (0x1234567855667788ULL, a, 1) != 0x88)
    __builtin_abort ();
  return 0;
}
