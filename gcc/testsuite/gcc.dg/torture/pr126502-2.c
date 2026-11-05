/* PR tree-optimization/126502 */
/* { dg-do run } */

/* The X <= Y ? 0 : X - Y form of an unsigned saturating subtraction has the
   same conversions as X >= Y ? X - Y : 0 and needs the same checks.  */

typedef __UINT16_TYPE__ u16;
typedef __UINT32_TYPE__ u32;
typedef __UINT64_TYPE__ u64;

#define NOIPA __attribute__ ((noipa))

NOIPA u64
wide_ternary (u32 x, u32 y)
{
  return x <= y ? 0 : (u64) x - y;
}

NOIPA u64
wide_if_else (u32 x, u32 y)
{
  u64 r;

  if (x <= y)
    r = 0;
  else
    r = x - y;
  return r;
}

NOIPA u32
narrow_minus_u16 (u32 x, u32 y)
{
  u16 d = (u16) x - (u16) y;

  return x <= y ? 0 : d;
}

NOIPA u16
trunc_u16 (u32 x, u32 y)
{
  return x <= y ? 0 : (u16) (x - y);
}

NOIPA u32
same_u32 (u32 x, u32 y)
{
  return x <= y ? 0 : x - y;
}

#if defined (__BITINT_MAXWIDTH__) && __BITINT_MAXWIDTH__ >= 27
NOIPA u64
bitint_from_u32 (u32 x, u32 y)
{
  unsigned _BitInt(27) r = x <= y ? 0 : (unsigned _BitInt(27)) (x - y);

  return r;
}
#endif

#define CHECK(expr, want) \
  do \
    if ((u64) (expr) != (u64) (want)) \
      __builtin_abort (); \
  while (0)

int
main (void)
{
  const u32 big = 3000000000u;

  CHECK (wide_ternary (big, 1), 2999999999u);
  CHECK (wide_ternary (1, 2), 0);
  CHECK (wide_ternary (2, 2), 0);
  CHECK (wide_if_else (big, 1), 2999999999u);
  CHECK (wide_if_else (1, 2), 0);

  CHECK (narrow_minus_u16 (0x20005, 1), 4);
  CHECK (narrow_minus_u16 (0x20000, 1), 0xffff);
  CHECK (narrow_minus_u16 (1, 2), 0);

  CHECK (trunc_u16 (0x20005, 1), 4);
  CHECK (trunc_u16 (1, 2), 0);
  CHECK (same_u32 (big, 1), 2999999999u);
  CHECK (same_u32 (1, 2), 0);

#if defined (__BITINT_MAXWIDTH__) && __BITINT_MAXWIDTH__ >= 27
  CHECK (bitint_from_u32 (0x8000005, 1), 4);
  CHECK (bitint_from_u32 (1, 2), 0);
#endif

  return 0;
}
