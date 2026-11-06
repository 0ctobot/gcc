/* PR target/86236 */
/* { dg-do compile { target lp64 } } */
/* { dg-require-effective-target fpic } */
/* { dg-options "-O2 -fPIC -ffixed-r10 -ffixed-r12 -ffixed-r13 -ffixed-r14 -ffixed-r15" } */

/* The GOT pointer doesn't occupy BX in 64-bit mode.  */

void u (void *, void *);

void
f (int n)
{
  char *a = __builtin_alloca (n);
  int x __attribute__ ((aligned (64)));
  u (a, &x);
}

/* { dg-final { scan-assembler "leaq\[ \t\]+\[0-9\]+\\(%rsp\\), %rbx" } } */
