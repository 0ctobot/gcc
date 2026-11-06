/* PR tree-optimization/85803 */
/* { dg-do run { target *-*-linux* *-*-gnu* } } */
/* { dg-options "-O3 -mgeneral-regs-only" } */
/* { dg-require-effective-target signal } */
/* { dg-require-effective-target sigsetjmp } */

/* The asm faults on purpose, with no memory clobber and no output.  The pair
   fusion pass used to put the first store into a pair behind the asm, so the
   signal handler didn't find it.  */

#include <setjmp.h>
#include <signal.h>

static long g[4];
static sigjmp_buf env;

static void
handler (int sig)
{
  siglongjmp (env, 1);
}

__attribute__ ((noinline)) void
f (int *bad)
{
  g[0] = 1;
  __asm__ volatile ("ldr wzr, [%0]" : : "r" (bad));
  g[1] = 2;
  g[2] = 3;
  g[3] = 4;
}

int *volatile badp;

int
main (void)
{
  signal (SIGSEGV, handler);
  signal (SIGBUS, handler);
  if (sigsetjmp (env, 1) == 0)
    {
      f (badp);
      __builtin_abort ();
    }
  if (*(volatile long *) &g[0] != 1)
    __builtin_abort ();
  return 0;
}
