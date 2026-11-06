/* { dg-do run } */
/* { dg-options "-Os -foptimize-sibling-calls" } */

/* A live local variable's address reached the recursive call without the call
   accessing the variable, so the call was turned into a jump.  That made the
   new invocation's local variable the same object as the old one.  */

extern void abort (void);

__attribute__ ((noinline)) const int *
identity (const int *a)
{
  return a;
}

int
by_pointer (int t, int x, const int *a)
{
  const int b[4] = { 0 };
  const int *c = identity (&b[0]);

  if (t == 0)
    return by_pointer (1, x, c);
  return c == a;
}

__attribute__ ((noinline)) long
identity_long (const int *a)
{
  return (long) a;
}

int
by_integer (int t, int x, long a)
{
  const int b[4] = { 0 };
  long c = identity_long (&b[0]);

  if (t == 0)
    return by_integer (1, x, c);
  return c == a;
}

const int *global;

int
through_global (int t)
{
  const int b[4] = { 0 };

  if (t == 0)
    {
      global = identity (&b[0]);
      return through_global (1);
    }
  return identity (&b[0]) == global;
}

int
main (void)
{
  if (by_pointer (0, 0, 0))
    abort ();
  if (by_integer (0, 0, 0))
    abort ();
  if (through_global (0))
    abort ();
  return 0;
}
