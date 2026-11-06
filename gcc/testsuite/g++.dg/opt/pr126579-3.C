/* { dg-do run { target { c++11 && *-*-linux* } } } */
/* { dg-options "-O2 -fnon-call-exceptions -fno-ssa-phiopt -fno-tree-cselim -fno-tree-sink" } */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <exception>

static void sigfpe_handler (int) { throw 1; }
static void my_terminate () { printf ("TERMINATE\n"); exit (0); }

volatile int zero = 0;

__attribute__((noipa)) void maythrow (void)
{
    if (zero) throw 2;
}

__attribute__((noipa))
long divsel (int c, long *a, long *b)
  noexcept
{
  long x = zero;
  if (c)
    *a = x;
  else
    *a = x + 1;
  return x;
}

int main (void)
{
  std::set_terminate (my_terminate);
  struct sigaction sa;
  sa.sa_handler = sigfpe_handler;
  sigemptyset (&sa.sa_mask);
  sa.sa_flags = SA_NODEFER;
  sigaction (SIGFPE, &sa, NULL);
  sigaction (SIGABRT, &sa, NULL);
  sigaction (SIGSEGV, &sa, NULL);

  try
    {
      maythrow ();
      int r = divsel (1, nullptr,  nullptr);
      printf ("NO TRAP r=%d\n", r);
      __builtin_exit (1);
    }
  catch (int)
    {
      printf ("ESCAPED noexcept, caught in main\n");
      __builtin_exit (1);
    }
  return 0;
}
