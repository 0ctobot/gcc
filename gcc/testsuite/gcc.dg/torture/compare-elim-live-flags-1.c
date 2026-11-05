/* { dg-do run } */
/* { dg-require-effective-target int32plus } */
/* { dg-require-effective-target size32plus } */

/* If-converting the diamond below leaves the subtraction's block falling
   straight into the loop header, which has two predecessors.  The flags from
   the comparison against zero are still read past the loop, so compare-elim
   must not merge that comparison into the subtraction: a subtraction that
   overflows sets the overflow flag, while the comparison always clears it, and
   that changes the result of `x > 0`.  */

typedef __UINT64_TYPE__ u64;
typedef __UINT32_TYPE__ u32;
typedef __INT32_TYPE__ s32;

__attribute__ ((noipa)) u64
f (u64 a, u64 b, u64 c, u64 d)
{
  s32 x = (s32) (u32) (a - b);
  unsigned int t = x > 0;
  u64 r = d - (b << 3);
  if (x != 0)
    r = a + (d << 2);
  u64 i = c;
  do
    {
      r += i;
      i >>= 1;
    }
  while (i);
  return r + t;
}

int
main (void)
{
  if (f (0x80000000u, 1, 0, 0) != 0x80000001u)
    __builtin_abort ();
  return 0;
}
