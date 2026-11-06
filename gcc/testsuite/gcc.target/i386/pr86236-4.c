/* PR target/86236 */
/* { dg-do compile { target lp64 } } */
/* { dg-options "-O2 -fno-pic -ffixed-r10 -ffixed-r12 -ffixed-r13 -ffixed-r14 -ffixed-r15 -ffixed-rbx" } */

/* With every register that has to be preserved taken, use one that doesn't
   hold an incoming argument.  The epilogue reloads the register, so one that
   carries part of the return value won't do either.  */

void u (void *, void *);

__int128
f1 (int n)
{
  char *a = __builtin_alloca (n);
  int x __attribute__ ((aligned (64)));
  u (a, &x);
  return n;
}

long
f2 (int n, long a2, long a3, long a4, long a5, long a6)
{
  char *a = __builtin_alloca (n);
  int x __attribute__ ((aligned (64)));
  u (a, &x);
  return a2 + a3 + a4 + a5 + a6;
}

/* { dg-final { scan-assembler-times "leaq\[ \t\]+\[0-9\]+\\(%rsp\\), %rsi" 1 } } */
/* { dg-final { scan-assembler-times "leaq\[ \t\]+\[0-9\]+\\(%rsp\\), %r11" 1 } } */
