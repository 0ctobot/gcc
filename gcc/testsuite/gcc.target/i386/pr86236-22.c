/* PR target/86236 */
/* { dg-do compile { target lp64 } } */
/* { dg-options "-O2 -mapxf -mgeneral-regs-only -fstack-clash-protection -ffixed-r10 -ffixed-r12 -ffixed-r13 -ffixed-r14 -ffixed-r15 -ffixed-rbx" } */
/* Every legacy register except AX is fixed or holds an argument, and stack
   clash protection rules out R11, but the APX registers are still free.  */
void u (void *);
volatile long n = 16;
void
f (long a, long b, long c, long d, long e, long g, long h)
{
  char *p = __builtin_alloca (n);
  char x[64] __attribute__ ((aligned (64)));
  u (p);
  u (x);
  u ((void *) (a + b + c + d + e + g + h));
}
/* { dg-final { scan-assembler "leaq\[ \t\]+8\\(%rsp\\), %r16" } } */
