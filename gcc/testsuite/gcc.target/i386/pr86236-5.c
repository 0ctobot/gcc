/* PR target/86236 */
/* { dg-do run { target lp64 } } */
/* { dg-require-effective-target supports_stack_clash_protection } */
/* { dg-options "-O2 -fstack-clash-protection" } */

/* The stack probe loop uses R11 before the stack arguments are read.  */

__attribute__ ((noipa)) void
u (void *a, void *b)
{
  __asm__ ("" : : "r" (a), "r" (b) : "memory");
}

__attribute__ ((noipa, preserve_none)) long
f (long a1, long a2, long a3, long a4, long a5, long a6, long a7, long a8,
   long a9, int n)
{
  char *a = __builtin_alloca (n);
  char x[20000] __attribute__ ((aligned (64)));
  u (a, x);
  return a7 + a8 + a9;
}

int
main (void)
{
  if (f (1, 2, 3, 4, 5, 6, 7, 8, 9, 16) != 24)
    __builtin_abort ();
  return 0;
}
