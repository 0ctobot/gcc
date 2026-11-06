/* A local variable that the named return value optimization turns into the
   function's result has to keep its automatic initialization, so that members
   the function doesn't write don't hand stale stack contents to the caller.  */
/* { dg-do run } */
/* { dg-options "-O2 -ftrivial-auto-var-init=zero" } */

struct big { long a[20]; };

__attribute__ ((noipa)) void
dirty_stack (void)
{
  volatile char buf[1024];
  for (int i = 0; i < 1024; i++)
    buf[i] = 0x5a;
}

__attribute__ ((noipa)) struct big
nrv (int set)
{
  struct big b;
  if (set)
    b.a[0] = 1;
  return b;
}

__attribute__ ((noipa)) void
check (void)
{
  struct big b = nrv (0);
  for (int i = 0; i < 20; i++)
    if (b.a[i])
      __builtin_abort ();
}

int
main (void)
{
  dirty_stack ();
  check ();
  return 0;
}
