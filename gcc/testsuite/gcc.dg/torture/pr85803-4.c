/* PR tree-optimization/85803 */
/* { dg-do run { target *-*-linux* *-*-gnu* } } */
/* { dg-additional-options "-fno-store-merging -fno-schedule-insns -fno-schedule-insns2" } */
/* { dg-require-effective-target signal } */
/* { dg-require-effective-target sigsetjmp } */

/* A store has to be in memory by the time a call faults, even when the call is
   known not to touch the memory that the store goes to.  */

#include <setjmp.h>
#include <signal.h>
#include <stddef.h>
#include <stdlib.h>

static sigjmp_buf env;
char *volatile badp;
volatile size_t n = 4;
static char buf[8];
static int g;
int sink;

__attribute__ ((noipa)) void
f1 (char *r, char *q)
{
  g = 42;
  __builtin_memcpy (r, q, n);
  g = 1;
}

__attribute__ ((noipa)) void
f2 (char *q)
{
  g = 42;
  __builtin_memset (q, 0, n);
  g = 1;
}

__attribute__ ((noipa)) size_t
f3 (char *q)
{
  g = 42;
  size_t v = __builtin_strlen (q);
  g = 1;
  return v;
}

__attribute__ ((noipa)) int
f4 (char *r, char *q)
{
  g = 42;
  int v = __builtin_memcmp (r, q, n);
  g = 1;
  return v;
}

__attribute__ ((noinline, noclone)) static int
load (char *q)
{
  return *q;
}

__attribute__ ((noipa)) int
f5 (char *q)
{
  g = 42;
  int v = load (q);
  g = 1;
  return v;
}

__attribute__ ((noinline, noclone)) static void
store (char *q)
{
  *q = 7;
}

__attribute__ ((noipa)) void
f6 (char *q)
{
  g = 42;
  store (q);
  g = 1;
}

static void
handler (int sig)
{
  siglongjmp (env, 1);
}

#define RUN(call)			\
  do					\
    {					\
      g = 0;				\
      if (sigsetjmp (env, 1) == 0)	\
	{				\
	  call;				\
	  abort ();			\
	}				\
      if (g != 42)			\
	abort ();			\
    }					\
  while (0)

int
main (void)
{
  struct sigaction sa;
  sa.sa_handler = handler;
  sa.sa_flags = 0;
  sigemptyset (&sa.sa_mask);
  sigaction (SIGSEGV, &sa, 0);

  RUN (f1 (buf, badp));
  RUN (f2 (badp));
  RUN (sink = f3 (badp));
  RUN (sink = f4 (buf, badp));
  RUN (sink = f5 (badp));
  RUN (f6 (badp));
  return 0;
}
