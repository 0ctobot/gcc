/* { dg-do run } */
/* { dg-options "-ftrivial-auto-var-init=zero" } */
/* { dg-require-effective-target indirect_jumps } */
/* { dg-require-effective-target label_values } */
/* { dg-require-effective-target nonlocal_goto } */

/* Like init-rerun-23.c, but the label below the declaration is reached by a
   computed goto, a goto out of a nested function or a longjmp.  Every user
   label counts as one that can skip the declaration in a function with a
   computed goto or a nested function, and no variable gets the automatic
   initialization ahead of an initializer in a function that calls setjmp.  */

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
  struct S s = { s.a + 1, 0 };
  return s.a;
 after:
  s.a = 41;
  s.b = 0;
  goto *targets[0];
}

__attribute__ ((noipa)) int
nonlocal_goto (void)
{
  __label__ after;
  __attribute__ ((noipa)) void skip (void) { goto after; }
  skip ();
 before:;
  struct S s = { s.a + 1, 0 };
  return s.a;
 after:
  s.a = 41;
  s.b = 0;
  goto before;
}

/* The longjmp enters the block of S again below its declaration.  */

__attribute__ ((noipa)) int
longjmp_in (void)
{
  volatile int n = 0;
  {
  top:;
    volatile struct S s = { n ? s.a + 1 : 0, 0 };
    if (n == 2)
      return s.a;
    if (setjmp (env))
      {
	s.a = 41;
	n = 2;
	goto top;
      }
  }
  n = 1;
  jump ();
  return -1;
}

/* The longjmp from above the declaration of S, in a later entry to its block,
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
    volatile struct S s = { n ? s.a + 1 : 0, 0 };
    if (n == 2)
      return s.a;
    if (setjmp (env))
      {
	s.a = 41;
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
  if (longjmp_in () != 42)
    __builtin_abort ();
  if (longjmp_down () != 42)
    __builtin_abort ();
  return 0;
}
