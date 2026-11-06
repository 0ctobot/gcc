/* { dg-do run } */
/* { dg-options "-ftrivial-auto-var-init=zero" } */

/* A goto or a case label that skips the declaration of a variable lets code
   below the declaration store to the variable before a jump back runs its
   initializer.  An initializer that gets built in a temporary has to read that
   stored value then, so the variable doesn't get the automatic initialization
   ahead of it.  */

struct S { int a, b; };

static struct S *p;

__attribute__ ((noipa)) int
read_p (void)
{
  return p->a + 1;
}

__attribute__ ((noipa)) int
by_name (void)
{
  goto after;
 before:;
  struct S s = { s.a + 1, 0 };
  return s.a;
 after:
  s.a = 41;
  s.b = 0;
  goto before;
}

__attribute__ ((noipa)) int
by_pointer (void)
{
  goto after;
 before:;
  struct S s = { read_p (), 0 };
  return s.a;
 after:
  p = &s;
  s.a = 41;
  s.b = 0;
  goto before;
}

__attribute__ ((noipa)) int
into_block (void)
{
  goto in;
  {
  top:;
    struct S s = { s.b + 1, 5 };
    return s.a;
  in:
    s.b = 41;
    goto top;
  }
}

__attribute__ ((noipa)) int
at_case (int k)
{
  switch (k)
    {
    case 0:
    again:;
      struct S s = { s.a + 1, 0 };
      return s.a;
    case 1:
      s.a = 41;
      s.b = 0;
      goto again;
    }
  return -1;
}

int
main (void)
{
  if (by_name () != 42)
    __builtin_abort ();
  if (by_pointer () != 42)
    __builtin_abort ();
  if (into_block () != 42)
    __builtin_abort ();
  if (at_case (1) != 42)
    __builtin_abort ();
  return 0;
}
