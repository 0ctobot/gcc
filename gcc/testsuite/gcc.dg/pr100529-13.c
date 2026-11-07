/* PR c/100529 */
/* { dg-do run } */
/* { dg-options "-std=gnu23" } */

/* As in pr100529-12.c, an arm stores through p before it declares its type
   sized from n, and p can point to n once n has its address taken after the
   conditional.  A loop or a call that can return twice runs the conditional
   again with p pointing to n, and the size of the result has to follow the
   store.  */

#include <setjmp.h>

extern void abort (void);

int *p;

__attribute__ ((noipa)) void
use (int *q)
{
  p = q;
}

#define ARM(x) ({ x; struct T { char a[n]; } t; t; })

__attribute__ ((noipa)) unsigned long
in_loop (int n, int c)
{
  unsigned long s = 0;
  for (int i = 0; i < 2; i++)
    {
      s = sizeof (c ? ARM (if (p) *p = 6) : ARM ());
      use (&n);
    }
  use (0);
  return s;
}

__attribute__ ((noipa)) int
in_loop_cond (int n, int c)
{
  int i = 0;
  while (sizeof (c ? ARM (if (p) *p = 6) : ARM ()) == 5 && i < 3)
    {
      use (&n);
      i++;
    }
  use (0);
  return i;
}

jmp_buf env;

__attribute__ ((noipa)) unsigned long
returns_twice (int n, int c)
{
  volatile int pass = 0;
  volatile unsigned long s;
  setjmp (env);
  s = sizeof (c ? ARM (if (p) *p = 6) : ARM ());
  use (&n);
  if (pass++ == 0)
    longjmp (env, 1);
  use (0);
  return s;
}

int
main (void)
{
  if (in_loop (5, 1) != 6 || in_loop (5, 0) != 5)
    abort ();
  if (in_loop_cond (5, 1) != 1 || in_loop_cond (5, 0) != 3)
    abort ();
  if (returns_twice (5, 1) != 6 || returns_twice (5, 0) != 5)
    abort ();
  return 0;
}
