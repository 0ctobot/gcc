/* PR tree-optimization/85803 */
/* { dg-do run { target *-*-linux* *-*-gnu* } } */
/* { dg-require-effective-target signal } */
/* { dg-require-effective-target sigsetjmp } */

/* A store has to be in memory by the time an access through another pointer
   faults, even though that isn't visible in the virtual use-def chain.  */

#include <setjmp.h>
#include <signal.h>
#include <stdlib.h>

static sigjmp_buf env;
int *volatile globp;
int g;

/* Taking the address of G keeps it from being proven distinct from what the
   functions below access through a pointer, even under LTO, in which case the
   scheduler would be free to move the store to G across that access.  */
int *volatile gp = &g;

static inline void __attribute__ ((always_inline))
inlinethrow (void *p)
{
  *globp = 43;
}

__attribute__ ((noinline, noclone)) int
f1 (int *p, int i)
{
  if (i)
    {
      if (i == 42)
	{
	  *p = 42;
	  int local __attribute__ ((cleanup (inlinethrow)));
	}
      *p = 0;
    }
  *p = 1;
  return i + 1;
}

__attribute__ ((noipa)) void
f2 (int *p, int *q)
{
  *p = 42;
  *q = 43;
  *p = 1;
}

__attribute__ ((noipa)) int
f3 (int *p, int *q)
{
  *p = 42;
  int v = *q;
  *p = 1;
  return v;
}

__attribute__ ((noipa)) void
f4 (int *q)
{
  g = 42;
  *q = 43;
  g = 1;
}

__attribute__ ((noipa)) int
f5 (int *q)
{
  g = 42;
  int v = *q;
  g = 1;
  return v;
}

static void
handler (int sig)
{
  siglongjmp (env, 1);
}

#define RUN(call, var)			\
  do					\
    {					\
      var = 0;				\
      if (sigsetjmp (env, 1) == 0)	\
	{				\
	  call;				\
	  abort ();			\
	}				\
      if (var != 42)			\
	abort ();			\
    }					\
  while (0)

int x;

int
main (void)
{
  struct sigaction sa;
  sa.sa_handler = handler;
  sa.sa_flags = 0;
  sigemptyset (&sa.sa_mask);
  sigaction (SIGSEGV, &sa, 0);

  RUN (f1 (&x, 42), x);
  RUN (f2 (&x, globp), x);
  RUN (f3 (&x, globp), x);
  RUN (f4 (globp), g);
  RUN (f5 (globp), g);
  return 0;
}
