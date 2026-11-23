/* { dg-do run } */
/* { dg-require-effective-target indirect_jumps } */

/* A callee can store through its argument, or let it escape, and then
   longjmp back into its caller.  The callee's summary leaves those effects
   out since they only happen on a path that doesn't return, but the caller
   sees them once setjmp returns again.  */

#include <setjmp.h>

static jmp_buf buf;
volatile int c = 1;
int *gp;

__attribute__((noinline, noclone, noreturn)) static void
jump_store (int *q)
{
  *q = 0;
  longjmp (buf, 1);
}

/* This can return normally, so its summary has no store and leaves Q
   neither clobbered nor escaping.  */

__attribute__((noinline, noclone)) static void
maybe_jump_store (int *q)
{
  if (c)
    jump_store (q);
}

__attribute__((noinline, noclone)) static int
f1 (void)
{
  int *x = __builtin_malloc (sizeof (int));

  *x = 1;
  if (setjmp (buf))
    return *x;
  maybe_jump_store (x);
  return -1;
}

/* This reads no memory, so it's found to be looping pure, and a call to it
   isn't seen to write memory at all.  */

__attribute__((noinline, noclone)) static void
maybe_jump_store_pure (int *q, int k)
{
  if (k)
    jump_store (q);
}

__attribute__((noinline, noclone)) static int
f2 (void)
{
  int *x = __builtin_malloc (sizeof (int));

  *x = 1;
  if (setjmp (buf))
    return *x;
  maybe_jump_store_pure (x, c);
  return -1;
}

__attribute__((noinline, noclone, noreturn)) static void
jump_escape (int *q)
{
  gp = q;
  longjmp (buf, 1);
}

__attribute__((noinline, noclone)) static void
maybe_jump_escape (int *q)
{
  if (c)
    jump_escape (q);
}

__attribute__((noinline, noclone)) static void
set3 (void)
{
  *gp = 3;
}

__attribute__((noinline, noclone)) static int
f3 (void)
{
  int *x = __builtin_malloc (sizeof (int));

  *x = 1;
  if (setjmp (buf))
    {
      *x = 2;
      set3 ();
      return *x;
    }
  maybe_jump_escape (x);
  return -1;
}

int
main (void)
{
  if (f1 () != 0)
    __builtin_abort ();
  if (f2 () != 0)
    __builtin_abort ();
  if (f3 () != 3)
    __builtin_abort ();
  return 0;
}
