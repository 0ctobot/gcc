/* PR target/86236 */
/* { dg-do compile { target lp64 } } */
/* { dg-options "-O2" } */

/* A stack adjustment too big for an immediate goes through R11 before the
   stack arguments are read.  */

void u (void *, void *);

__attribute__ ((preserve_none)) long
f (long a1, long a2, long a3, long a4, long a5, long a6, long a7, int n)
{
  char *a = __builtin_alloca (n);
  char x[3000000000] __attribute__ ((aligned (64)));
  u (a, x);
  return a7;
}

/* { dg-final { scan-assembler-not "leaq\[ \t\]+\[0-9\]+\\(%rsp\\), %r11" } } */
/* { dg-final { scan-assembler "leaq\[ \t\]+\[0-9\]+\\(%rsp\\), %rbx" } } */
