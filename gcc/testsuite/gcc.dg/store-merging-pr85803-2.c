/* PR tree-optimization/85803 */
/* { dg-do run { target *-*-linux* *-*-gnu* } } */
/* { dg-require-effective-target store_merge } */
/* { dg-require-effective-target signal } */
/* { dg-require-effective-target sigsetjmp } */
/* { dg-options "-O2 -fno-schedule-insns -fno-schedule-insns2 -fno-schedule-fusion" } */

/* Store merging deletes a store that a later store in its group overwrites,
   which isn't valid across a statement that may fault on something else,
   because a signal handler gets to see what the first store wrote.  */

#include <setjmp.h>
#include <signal.h>
#include <stddef.h>
#include <stdlib.h>

static sigjmp_buf env;
char *volatile badp;
static int g;
int sink;

__attribute__ ((noipa)) void
f1 (char *q)
{
  g = 42;
  *(int *) q = 7;
  g = 1;
}

__attribute__ ((noipa)) void
f2 (char *q)
{
  g = 42;
  sink = *(int *) q;
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

__attribute__ ((noinline, noclone)) static void
store (char *q)
{
  *q = 7;
}

__attribute__ ((noipa)) void
f4 (char *q)
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

  RUN (f1 (badp));
  RUN (f2 (badp));
  RUN (sink = f3 (badp));
  RUN (f4 (badp));
  return 0;
}
