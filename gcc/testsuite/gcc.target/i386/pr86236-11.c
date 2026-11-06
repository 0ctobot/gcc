/* PR target/86236 */
/* { dg-do run { target lp64 } } */
/* { dg-options "-O2 -ffixed-r10 -ffixed-r12 -ffixed-r13 -ffixed-r14 -ffixed-r15 -ffixed-rbx -save-temps" } */

/* DI doesn't hold a return value, so a function that doesn't take an argument
   in it can use it for the DRAP register.  */

struct big { long a[4]; };

__attribute__ ((noipa)) void
u (void *a, void *b)
{
  __asm__ ("" : : "r" (a), "r" (b) : "memory");
}

__attribute__ ((noipa)) long
f (struct big s)
{
  char *a = __builtin_alloca (32);
  int x __attribute__ ((aligned (64)));
  u (a, &x);
  return s.a[0] + s.a[3];
}

int
main (void)
{
  struct big s = { { 3, 0, 0, 4 } };

  if (f (s) != 7)
    __builtin_abort ();
  return 0;
}

/* { dg-final { scan-assembler "leaq\[ \t\]+\[0-9\]+\\(%rsp\\), %rdi" } } */
