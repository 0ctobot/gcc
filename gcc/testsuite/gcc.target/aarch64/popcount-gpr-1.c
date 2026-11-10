/* { dg-do compile } */
/* { dg-options "-O2 -mgeneral-regs-only" } */

/* Without SIMD or CSSC, popcount and parity must expand inline in the
   general registers rather than call __popcountsi2/__popcountdi2 or
   __paritysi2/__paritydi2, which the Linux kernel doesn't provide.  */

#pragma GCC target "+nocssc"

int
pc32 (unsigned int x)
{
  return __builtin_popcount (x);
}

int
pc64 (unsigned long long x)
{
  return __builtin_popcountll (x);
}

int
par32 (unsigned int x)
{
  return __builtin_parity (x);
}

int
par64 (unsigned long long x)
{
  return __builtin_parityll (x);
}

/* { dg-final { scan-assembler-not {__popcount} } } */
/* { dg-final { scan-assembler-not {__parity} } } */
/* { dg-final { scan-assembler-not {\tcnt\t} } } */
/* { dg-final { scan-assembler-not {\tbl\t} } } */
/* { dg-final { scan-assembler-times {\tmul\tw[0-9]+, w[0-9]+, w[0-9]+\n} 2 } } */
/* { dg-final { scan-assembler-times {\tmul\tx[0-9]+, x[0-9]+, x[0-9]+\n} 2 } } */
