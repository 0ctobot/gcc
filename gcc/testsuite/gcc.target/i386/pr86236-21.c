/* PR target/86236 */
/* { dg-do compile { target ia32 } } */
/* { dg-options "-O2 -mforce-drap -mgeneral-regs-only -ffixed-edi -ffixed-esi -ffixed-ebx -ffixed-ecx" } */

/* Varargs functions don't pass anything in registers on ia32, so DX can be
   the DRAP register.  */

long
va (int n, ...)
{
  __builtin_va_list ap;
  char x[64] __attribute__ ((aligned (64)));
  __builtin_va_start (ap, n);
  __asm__ volatile ("" : : "r" (x) : "memory");
  long r = __builtin_va_arg (ap, long);
  __builtin_va_end (ap);
  return r;
}

/* { dg-final { scan-assembler "leal\[ \t\]+\[0-9\]+\\(%esp\\), %edx" } } */
