/* { dg-do compile } */
/* { dg-options "-O2 -mgeneral-regs-only" } */

/* Without SIMD or CSSC, a 128-bit popcount or parity must also expand
   inline, through the DImode popcount.  */

#pragma GCC target "+nocssc"

int
pc128 (unsigned __int128 x)
{
  return __builtin_popcountg (x);
}

int
par128 (unsigned __int128 x)
{
  return __builtin_parityg (x);
}

/* { dg-final { scan-assembler-not {__popcount} } } */
/* { dg-final { scan-assembler-not {__parity} } } */
/* { dg-final { scan-assembler-not {\tcnt\t} } } */
/* { dg-final { scan-assembler-not {\tbl\t} } } */
