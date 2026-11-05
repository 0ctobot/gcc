/* PR target/126555: SUBP sign-extends bits [55:0] of each operand, so it
   must not be used for a subtraction of zero-extended 56-bit values.  */

/* { dg-do compile } */
/* { dg-options "-O2 -march=armv8.5-a+memtag" } */

long
f (long a, long b)
{
  return (a & 0x00ffffffffffffffL) - (b & 0x00ffffffffffffffL);
}

/* { dg-final { scan-assembler-not {\tsubp\t} } } */
/* { dg-final { scan-assembler-times {\tand\t} 2 } } */
