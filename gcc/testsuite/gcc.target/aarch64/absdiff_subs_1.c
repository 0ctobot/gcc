/* { dg-do compile } */
/* { dg-options "-O2" } */

/* An absolute difference computes both differences ahead of the compare.
   The compare should be folded into the subtraction it duplicates.  */

typedef unsigned long long u64;
typedef unsigned int u32;
typedef long long s64;
typedef int s32;

u64
ad64 (u64 a, u64 b)
{
  return a > b ? a - b : b - a;
}

u32
ad32 (u32 a, u32 b)
{
  return a > b ? a - b : b - a;
}

u64
ad64_ge (u64 a, u64 b)
{
  return a >= b ? a - b : b - a;
}

s64
ads64 (s64 a, s64 b)
{
  return a > b ? a - b : b - a;
}

s32
ads32 (s32 a, s32 b)
{
  return a > b ? a - b : b - a;
}

/* { dg-final { scan-assembler-times {\tsubs\tx[0-9]+, x[0-9]+, x[0-9]+\n} 3 } } */
/* { dg-final { scan-assembler-times {\tsubs\tw[0-9]+, w[0-9]+, w[0-9]+\n} 2 } } */
/* { dg-final { scan-assembler-not {\tcmp\t} } } */
