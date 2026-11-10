/* { dg-do run } */
/* { dg-additional-options "-ftrivial-auto-var-init=zero -Wno-uninitialized" } */
/* { dg-require-effective-target indirect_jumps } */
/* { dg-require-effective-target label_values } */
/* { dg-require-effective-target nonlocal_goto } */

/* Like auto-init-self-5.c, but the label below the declaration is reached by
   a computed goto, a goto out of a nested function or a longjmp.  Every user
   label counts as one that can skip the declaration in a function with a
   computed goto or a nested function.  A longjmp can also go back above the
   declaration after code below it has stored to the variable, so no variable
   gets the automatic initialization ahead of an initializer in a function that
   calls setjmp.  */

#include <setjmp.h>

struct S { int a, b; };

static jmp_buf env;

__attribute__ ((noipa)) void
jump (void)
{
  longjmp (env, 1);
}

__attribute__ ((noipa)) int
computed_goto (int k)
{
  static void *const targets[] = { &&before, &&after };
  goto *targets[k];
 before:;
  int x = x + 1;
  return x;
 after:
  x = 41;
  goto *targets[0];
}

__attribute__ ((noipa)) int
nonlocal_goto (void)
{
  __label__ after;
  __attribute__ ((noipa)) void skip (void) { goto after; }
  skip ();
 before:;
  int x = x + 1;
  return x;
 after:
  x = 41;
  goto before;
}

__attribute__ ((noipa)) int
longjmp_back (void)
{
  volatile int again = 0;
  if (setjmp (env))
    again = 1;
  volatile int x = again ? x + 1 : 0;
  if (!again)
    {
      x = 41;
      jump ();
    }
  return x;
}

__attribute__ ((noipa)) int
longjmp_back_aggregate (void)
{
  volatile int again = 0;
  if (setjmp (env))
    again = 1;
  volatile struct S s = { again ? s.a + 1 : 0, 0 };
  if (!again)
    {
      s.a = 41;
      jump ();
    }
  return s.a;
}

/* The longjmp enters the block of X again below its declaration.  */

__attribute__ ((noipa)) int
longjmp_in (void)
{
  volatile int n = 0;
  {
  top:;
    volatile int x = n ? x + 1 : 0;
    if (n == 2)
      return x;
    if (setjmp (env))
      {
	x = 41;
	n = 2;
	goto top;
      }
  }
  n = 1;
  jump ();
  return -1;
}

/* The longjmp from above the declaration of X, in a later entry to its block,
   lands below the declaration.  */

__attribute__ ((noipa)) int
longjmp_down (void)
{
  volatile int n = 0;
  volatile int pass = 0;
 again:
  {
    if (pass == 1)
      jump ();
  top:;
    volatile int x = n ? x + 1 : 0;
    if (n == 2)
      return x;
    if (setjmp (env))
      {
	x = 41;
	n = 2;
	goto top;
      }
  }
  pass = 1;
  goto again;
}

int
main (void)
{
  if (computed_goto (1) != 42)
    __builtin_abort ();
  if (nonlocal_goto () != 42)
    __builtin_abort ();
  if (longjmp_back () != 42)
    __builtin_abort ();
  if (longjmp_back_aggregate () != 42)
    __builtin_abort ();
  if (longjmp_in () != 42)
    __builtin_abort ();
  if (longjmp_down () != 42)
    __builtin_abort ();
  return 0;
}
