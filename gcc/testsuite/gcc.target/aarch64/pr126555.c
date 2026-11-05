/* PR target/126555: SUBP sign-extends bits [55:0] of each operand, so it
   must not be used for a subtraction of zero-extended 56-bit values.  */

/* { dg-do compile } */
/* { dg-options "-O2 -march=armv8.5-a+memtag" } */

__INT64_TYPE__
f (__INT64_TYPE__ a, __INT64_TYPE__ b)
{
  return (a & 0x00ffffffffffffffLL) - (b & 0x00ffffffffffffffLL);
}

/* { dg-final { scan-assembler-not {\tsubp\t} } } */
/* { dg-final { scan-assembler-times {\tand\t} 2 } } */
