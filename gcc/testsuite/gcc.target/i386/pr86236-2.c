/* PR target/86236 */
/* { dg-do compile { target lp64 } } */
/* { dg-options "-O2 -mforce-drap -mstackrealign" } */

void f1 (void *, long);
register long r10 __asm__ ("r10"); /* { dg-warning "call-clobbered register" } */

void
f2 (void)
{
  char m[64] __attribute__ ((aligned (64)));
  f1 (m, r10);
}

/* The argument pointer must not clobber the global register variable.  */
/* { dg-final { scan-assembler-not "leaq\[ \t\]+\[0-9\]+\\(%rsp\\), %r10" } } */
/* { dg-final { scan-assembler "leaq\[ \t\]+\[0-9\]+\\(%rsp\\), %r13" } } */
