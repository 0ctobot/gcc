/* PR target/86236 */
/* { dg-do compile { target lp64 } } */
/* { dg-require-effective-target split_stack } */
/* { dg-options "-O2 -fsplit-stack -mgeneral-regs-only -mforce-drap -mstackrealign -ffixed-r10 -ffixed-r12 -ffixed-r13 -ffixed-r14 -ffixed-r15 -ffixed-rbx" } */

/* With -fsplit-stack, a varargs function gets a pointer to its stack
   arguments from the split-stack prologue in R11, and the body only copies it
   out after the DRAP register is loaded.  Every other register is fixed or can
   carry an argument, so no register is left for the DRAP.  */

long
sum (int n, ...)
{
  volatile long al __attribute__ ((aligned (64))) = 0;
  __builtin_va_list ap;
  long s = al;
  __builtin_va_start (ap, n);
  for (int i = 0; i < n; i++)
    s += __builtin_va_arg (ap, long);
  __builtin_va_end (ap);
  return s;
} /* { dg-message "no register is available" } */
