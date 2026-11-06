/* PR target/86236 */
/* { dg-do run { target lp64 } } */
/* { dg-options "-O2 -mgeneral-regs-only -mforce-drap -mstackrealign -ffixed-rdi -ffixed-rsi -ffixed-r10 -ffixed-r12 -ffixed-r13 -ffixed-r14 -ffixed-r15 -ffixed-rbx" } */

/* A function that is both ms_abi and preserve_none takes its arguments in the
   MS ABI registers, so R9 holds its fourth argument and can't be the DRAP
   register.  */

__attribute__ ((noipa, ms_abi, preserve_none)) long
f (long a, long b, long c, long d)
{
  char x[64] __attribute__ ((aligned (64)));
  __asm__ volatile ("" : : "r" (x) : "memory");
  return a + b * 2 + c * 3 + d * 4;
}

int
main (void)
{
  if (f (1, 2, 3, 4) != 30)
    __builtin_abort ();
  return 0;
}
