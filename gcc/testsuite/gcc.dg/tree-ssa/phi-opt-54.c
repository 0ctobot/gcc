/* { dg-do compile } */
/* { dg-require-effective-target int32plus } */
/* { dg-options "-O2 -fdump-tree-optimized" } */

/* Bits tested through a helper returning int, as with the swap pte flag
   helpers in the Linux kernel, then cleared in the tested value or copied
   to another one.  None of this needs a branch or a conditional move.  */

typedef __UINT64_TYPE__ u64;
typedef __UINT32_TYPE__ u32;

static inline int bit2 (u64 x) { return x & 4; }
static inline int bit3 (u64 x) { return !!(x & 8); }

u64
clear_bits (u64 x)
{
  if (bit2 (x))
    x &= ~4ULL;
  if (bit3 (x))
    x &= ~8ULL;
  return x;
}

u64
clear_bits_inverted (u64 x)
{
  if (!bit2 (x))
    return x;
  return x & ~4ULL;
}

u64
copy_bits (u64 x, u64 y)
{
  if (bit2 (x))
    y |= 4;
  if (bit3 (x))
    y |= 8;
  return y;
}

u64
copy_bits_inverted (u64 x, u64 y)
{
  if (!bit2 (x))
    return y;
  return y | 4;
}

u32
copy_to_narrower (u64 x, u32 y)
{
  if (x & (1ULL << 31))
    y |= 1U << 31;
  return y;
}

u64
copy_to_wider (int x, u64 y)
{
  if (x & 0x40000000)
    y |= 0x40000000;
  return y;
}

/* { dg-final { scan-tree-dump-not "if " "optimized" } } */
/* { dg-final { scan-tree-dump-times "& 18446744073709551603;" 1 "optimized" } } */
/* { dg-final { scan-tree-dump-times "& 18446744073709551611;" 1 "optimized" } } */
