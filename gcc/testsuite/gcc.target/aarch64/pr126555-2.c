/* PR target/126555: SUBP of two operands that are sign-extended from bit 55
   is what the instruction does, so it can still be used for that.  */

/* { dg-do compile } */
/* { dg-options "-O2 -march=armv8.5-a+memtag" } */

long
f (long a, long b)
{
  return ((a << 8) >> 8) - ((b << 8) >> 8);
}

/* { dg-final { scan-assembler-times {\tsubp\t} 1 } } */
