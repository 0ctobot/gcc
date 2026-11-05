/* PR tree-optimization/126502 */
/* { dg-do run } */

typedef __UINT8_TYPE__ u8;
typedef __UINT16_TYPE__ u16;
typedef __UINT32_TYPE__ u32;
typedef __UINT64_TYPE__ u64;

#define NOIPA __attribute__ ((noipa))

/* The result is wider than the operands, so it must be zero-extended rather
   than sign-extended from the saturating subtraction's mode.  */

NOIPA u64
wide_ternary (u32 x, u32 y)
{
  return x >= y ? (u64) x - y : 0;
}

NOIPA u64
wide_narrow_minus (u32 x, u32 y)
{
  return x >= y ? (u64) (x - y) : 0;
}

NOIPA u64
wide_if_else (u32 x, u32 y)
{
  u64 r;

  if (x >= y)
    r = x - y;
  else
    r = 0;
  return r;
}

NOIPA u64
wide_zero_init (u32 x, u32 y)
{
  u64 r = 0;

  if (x >= y)
    r = x - y;
  return r;
}

NOIPA u64
wide_mult (u32 x, u32 y)
{
  return (u64) (x - y) * (x >= y);
}

NOIPA u32
wide_u16 (u16 x, u16 y)
{
  return x >= y ? (u32) x - y : 0;
}

NOIPA u64
wide_u8 (u8 x, u8 y)
{
  return x >= y ? (u64) x - y : 0;
}

/* The program asks for a sign extension here, which the zero-extending
   vectorizer path of the saturating subtraction would lose.  */

NOIPA u64
wide_signed_minus (u32 x, u32 y)
{
  return x >= y ? (u64) (__INT32_TYPE__) (x - y) : 0;
}

/* The subtraction is narrower than the result, so the upper bits of the
   difference have to be discarded.  */

NOIPA u32
narrow_minus_u16 (u32 x, u32 y)
{
  u16 d = (u16) x - (u16) y;

  return x >= y ? d : 0;
}

NOIPA u64
narrow_minus_u32 (u64 x, u64 y)
{
  u32 d = (u32) x - (u32) y;

  return x >= y ? d : 0;
}

NOIPA u32
narrow_minus_if (u32 x, u32 y)
{
  u32 r = 0;

  if (x >= y)
    r = (u16) (x - y);
  return r;
}

/* Truncating the saturated difference is correct and has to keep working.  */

NOIPA u32
trunc_u32 (u64 x, u64 y)
{
  return x >= y ? (u32) x - (u32) y : 0;
}

NOIPA u16
trunc_u16 (u32 x, u32 y)
{
  return x >= y ? (u16) (x - y) : 0;
}

NOIPA u8
trunc_u8 (u64 x, u64 y)
{
  return x >= y ? (u8) (x - y) : 0;
}

NOIPA u32
same_u32 (u32 x, u32 y)
{
  return x >= y ? x - y : 0;
}

NOIPA u64
same_u64 (u64 x, u64 y)
{
  return x >= y ? x - y : 0;
}

/* The saturating subtraction doesn't reduce its result to a precision below
   that of the result's mode.  */

#if defined (__BITINT_MAXWIDTH__) && __BITINT_MAXWIDTH__ >= 27
NOIPA u64
bitint_from_u32 (u32 x, u32 y)
{
  unsigned _BitInt(27) r = x >= y ? (unsigned _BitInt(27)) (x - y) : 0;

  return r;
}

NOIPA u64
bitint_from_u64 (u64 x, u64 y)
{
  unsigned _BitInt(27) r = x >= y ? (unsigned _BitInt(27)) (x - y) : 0;

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
  CHECK (wide_narrow_minus (big, 1), 2999999999u);
  CHECK (wide_narrow_minus (1, 2), 0);
  CHECK (wide_if_else (big, 1), 2999999999u);
  CHECK (wide_if_else (1, 2), 0);
  CHECK (wide_zero_init (big, 1), 2999999999u);
  CHECK (wide_zero_init (1, 2), 0);
  CHECK (wide_mult (big, 1), 2999999999u);
  CHECK (wide_mult (1, 2), 0);
  CHECK (wide_u16 (60000, 1), 59999);
  CHECK (wide_u16 (1, 2), 0);
  CHECK (wide_u8 (200, 1), 199);
  CHECK (wide_u8 (1, 2), 0);
  CHECK (wide_signed_minus (big, 1), (u64) (__INT32_TYPE__) 2999999999u);
  CHECK (wide_signed_minus (1, 2), 0);

  CHECK (narrow_minus_u16 (0x20005, 1), 4);
  CHECK (narrow_minus_u16 (0x20000, 1), 0xffff);
  CHECK (narrow_minus_u16 (1, 2), 0);
  CHECK (narrow_minus_u32 (0x200000005ull, 1), 4);
  CHECK (narrow_minus_u32 (0x200000000ull, 1), 0xffffffffu);
  CHECK (narrow_minus_u32 (1, 2), 0);
  CHECK (narrow_minus_if (0x20005, 1), 4);
  CHECK (narrow_minus_if (1, 2), 0);

  CHECK (trunc_u32 (0x200000005ull, 1), 4);
  CHECK (trunc_u32 (1, 2), 0);
  CHECK (trunc_u32 (0xffffffffull, 0x100000000ull), 0);
  CHECK (trunc_u16 (0x20005, 1), 4);
  CHECK (trunc_u16 (1, 2), 0);
  CHECK (trunc_u8 (0x20005, 1), 4);
  CHECK (trunc_u8 (1, 2), 0);
  CHECK (same_u32 (big, 1), 2999999999u);
  CHECK (same_u32 (1, 2), 0);
  CHECK (same_u64 (0xc000000000000000ull, 1), 0xbfffffffffffffffull);
  CHECK (same_u64 (1, 2), 0);

#if defined (__BITINT_MAXWIDTH__) && __BITINT_MAXWIDTH__ >= 27
  CHECK (bitint_from_u32 (0x8000005, 1), 4);
  CHECK (bitint_from_u32 (1, 2), 0);
  CHECK (bitint_from_u64 (0x8000005, 1), 4);
  CHECK (bitint_from_u64 (1, 2), 0);
#endif

  return 0;
}
