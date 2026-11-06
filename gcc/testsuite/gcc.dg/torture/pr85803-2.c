/* PR tree-optimization/85803 */
/* { dg-do run { target *-*-linux* *-*-gnu* } } */
/* { dg-require-effective-target signal } */
/* { dg-require-effective-target sigsetjmp } */

/* A function that stays around forever has to make the stores in front of the
   point where it stays around visible, since something else can read them while
   it runs.  This uses a timer signal to look at the memory.  */

#include <setjmp.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/time.h>

static sigjmp_buf jb;
static int *volatile watch;
static volatile int want;
static volatile int seen;
static volatile int ticks;
volatile int vz;
int one = 1;

static void
handler (int sig)
{
  /* Give the function another chance if it hasn't reached its stores yet.  */
  seen = *(volatile int *) watch;
  if (seen == want || ++ticks == 5)
    siglongjmp (jb, 1);
}

extern void publish (int *);

__attribute__ ((noinline, noclone)) void
publish (int *p)
{
  watch = p;
}

int x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11;
int y;

/* A path to an endless loop next to a path to a kill.  */
__attribute__ ((noinline, noclone)) void
f1 (int c)
{
  x1 = 42;
  if (c)
    for (;;);
  x1 = 0;
}

/* An endless loop that has a virtual definition.  */
__attribute__ ((noinline, noclone)) void
f2 (int c)
{
  x2 = 42;
  if (c)
    for (;;)
      y = 1;
  x2 = 0;
}

/* Store sinking past a branch to an endless loop.  */
__attribute__ ((noinline, noclone)) void
f3 (int e, int c)
{
  if (e)
    {
      x3 = 42;
      if (c)
	for (;;);
    }
  else
    y = 1;
}

__attribute__ ((noinline, noclone)) int
f4 (int c)
{
  x4 = 42;
  if (c)
    for (;;);
  return x4;
}

/* Common store sinking.  */
__attribute__ ((noinline, noclone)) void
f5 (int a, int c)
{
  if (a)
    {
      x5 = 42;
      if (c)
	for (;;);
    }
  else
    x5 = 2;
}

/* Store motion out of a loop that doesn't always finish.  */
__attribute__ ((noinline, noclone)) void
f6 (int n)
{
  for (int i = 0; i < n; i++)
    {
      x6 = 42 + i;
      if (one)
	for (;;);
    }
}

/* Store motion out of a loop without any exit.  */
__attribute__ ((noinline, noclone)) void
f7 (void)
{
  for (;;)
    x7 = 42;
}

/* A callee that stays around without a side effect on memory.  */
__attribute__ ((noinline)) static void
hang1 (int c)
{
  if (c)
    for (;;);
}

__attribute__ ((noinline, noclone)) void
f8 (int c)
{
  x8 = 42;
  hang1 (c);
  x8 = 0;
}

/* A callee that only spins on an unrelated volatile.  */
__attribute__ ((noinline)) static void
hang2 (int c)
{
  while (c && !vz);
}

__attribute__ ((noinline, noclone)) void
f9 (int c)
{
  x9 = 42;
  hang2 (c);
  x9 = 0;
}

/* Spin loops that have an exit, which aren't assumed to finish.  */
__attribute__ ((noinline, noclone)) void
f10 (int c)
{
  x10 = 42;
  while (c)
    __asm__ volatile ("");
  x10 = 0;
}

__attribute__ ((noinline, noclone)) void
f11 (void)
{
  x11 = 42;
  while (!vz);
  x11 = 0;
}

/* Local variables that something else got the address of.  */
__attribute__ ((noinline, noclone)) void
f12 (void)
{
  int l;
  publish (&l);
  l = 42;
  for (;;);
}

__attribute__ ((noinline, noclone)) void
f13 (int c)
{
  int l;
  publish (&l);
  l = 42;
  if (c)
    for (;;);
  l = 0;
  publish (&l);
}

__attribute__ ((noinline, noclone)) void
f14 (void)
{
  int l;
  publish (&l);
  l = 42;
  while (!vz);
}

__attribute__ ((noinline, noclone)) void
f15 (void)
{
  int l;
  publish (&l);
  l = 42;
  hang2 (1);
}

static struct itimerval tv_on = { { 0, 20000 }, { 0, 20000 } };
static struct itimerval tv_off;

#define RUN(VAR, CALL)						\
  do								\
    {								\
      watch = VAR;						\
      want = 42;						\
      ticks = 0;						\
      if (!sigsetjmp (jb, 1))					\
	{							\
	  if (setitimer (ITIMER_VIRTUAL, &tv_on, NULL))		\
	    return 0;						\
	  CALL;							\
	  abort ();						\
	}							\
      setitimer (ITIMER_VIRTUAL, &tv_off, NULL);		\
      if (seen != want)						\
	abort ();						\
    }								\
  while (0)

int
main (void)
{
  static int dummy;
  struct sigaction sa = { 0 };

  sa.sa_handler = handler;
  sigemptyset (&sa.sa_mask);
  if (sigaction (SIGVTALRM, &sa, NULL))
    return 0;

  RUN (&x1, f1 (1));
  RUN (&x2, f2 (1));
  RUN (&x3, f3 (1, 1));
  RUN (&x4, f4 (1));
  RUN (&x5, f5 (1, 1));
  RUN (&x6, f6 (10));
  RUN (&x7, f7 ());
  RUN (&x8, f8 (1));
  RUN (&x9, f9 (1));
  RUN (&x10, f10 (1));
  RUN (&x11, f11 ());
  RUN (&dummy, f12 ());
  RUN (&dummy, f13 (1));
  RUN (&dummy, f14 ());
  RUN (&dummy, f15 ());
  return 0;
}
