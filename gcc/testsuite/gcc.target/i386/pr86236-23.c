/* PR target/86236 */
/* { dg-do run { target lp64 } } */
/* { dg-options "-O2 -fstack-clash-protection -ffixed-r10 -ffixed-r12 -ffixed-r13 -ffixed-r14 -ffixed-r15 -ffixed-rbx" } */
/* AX is the only register left for the DRAP register, and the seventh
   argument has to be read through it.  */
volatile long n = 16;
volatile long sum;

__attribute__ ((noipa)) void
u (void *p)
{
  __asm__ volatile ("" : : "r" (p) : "memory");
}

__attribute__ ((noipa)) void
f (long a, long b, long c, long d, long e, long g, long h)
{
  char *p = __builtin_alloca (n);
  char x[64] __attribute__ ((aligned (64)));
  u (p);
  u (x);
  if ((unsigned long) x & 63)
    __builtin_abort ();
  sum = a + b + c + d + e + g + h;
}

int
main (void)
{
  f (1, 2, 3, 4, 5, 6, 7);
  if (sum != 28)
    __builtin_abort ();
  return 0;
}
