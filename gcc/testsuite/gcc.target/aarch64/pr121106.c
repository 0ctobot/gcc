/* PR middle-end/121106 */
/* Inlining f () into g () brings in a QImode .POPCOUNT that was valid for
   the callee, but CSSC only provides popcount insns for SImode and DImode.  */
/* { dg-do compile } */
/* { dg-options "-O2" } */

static int
f (unsigned char x)
{
  return __builtin_popcount (x);
}

__attribute__((target ("+cssc"))) int
g (unsigned char x)
{
  return f (x) + 1;
}

/* { dg-final { scan-assembler {\tcnt\tw[0-9]+, w[0-9]+\n} } } */
/* { dg-final { scan-assembler-not {__popcount} } } */
