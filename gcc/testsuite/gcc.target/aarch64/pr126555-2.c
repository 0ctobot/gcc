/* PR target/126555: SUBP of two operands that are sign-extended from bit 55
   is what the instruction does, so it can still be used for that.  */

/* { dg-do compile } */
/* { dg-options "-O2 -march=armv8.5-a+memtag" } */

__INT64_TYPE__
f (__INT64_TYPE__ a, __INT64_TYPE__ b)
{
  __INT64_TYPE__ x = (__UINT64_TYPE__) a << 8;
  __INT64_TYPE__ y = (__UINT64_TYPE__) b << 8;
  return (x >> 8) - (y >> 8);
}

/* { dg-final { scan-assembler-times {\tsubp\t} 1 } } */
