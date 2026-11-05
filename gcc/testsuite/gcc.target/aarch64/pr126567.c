/* PR tree-optimization/126567 */
/* { dg-do run { target aarch64*-*-linux* } } */
/* { dg-options "-O2" } */

/* Nothing after the merge block touches memory, so it gets no virtual PHI.
   The load of *p in the first arm must still read the value from before
   the store that follows it.  The result leaves through an exit syscall
   since anything that keeps memory live would bring the virtual PHI back.  */

int a, b;

#define EXIT_WITH(x) \
  __asm__ __volatile__ ("mov x8, 94\n\tmov w0, %w0\n\tsvc 0" \
			:: "r" (x) : "x0", "x8")

__attribute__ ((noipa, noreturn)) static void
f (int c, int *p, int *r, int want)
{
  int v;

  if (c)
    {
      v = *p;
      *p = 7;
    }
  else
    v = *r;

  for (;;)
    EXIT_WITH (v != want);
}

int
main (void)
{
  volatile int five = 5, nine = 9;

  a = five;
  b = nine;
  f (1, &a, &b, five);
}
