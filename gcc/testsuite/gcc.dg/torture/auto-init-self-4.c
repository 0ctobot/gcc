/* { dg-do run } */
/* { dg-additional-options "-ftrivial-auto-var-init=zero -Wno-uninitialized" } */

/* A nested function reaches a variable of its parent through the static
   chain, so it can store to the variable before control reaches the
   declaration of the variable.  An initializer that reads the variable has to
   see that value, so the variable doesn't get zeroed ahead of the
   initializer.  A re-run of the initializer after a goto back sees the value
   of the previous run as usual.  */

struct S { int a, b; };

__attribute__((noipa)) int
scalar (void)
{
  auto void __attribute__((noipa)) set (void);
  set ();
  int x = x + 1;
  void __attribute__((noipa)) set (void) { x = 41; }
  return x;
}

__attribute__((noipa)) int
aggregate (void)
{
  auto void __attribute__((noipa)) set (void);
  set ();
  struct S s = { s.a + 1, s.b + 2 };
  void __attribute__((noipa)) set (void) { s.a = 41; s.b = 40; }
  return s.a + s.b;
}

__attribute__((noipa)) int
through_address (void)
{
  int *p;
  auto void __attribute__((noipa)) set (void);
  set ();
  int x = *p + 1;
  void __attribute__((noipa)) set (void) { p = &x; *p = 41; }
  return x;
}

__attribute__((noipa)) int
rerun (void)
{
  volatile int pass = 0;
  auto void __attribute__((noipa)) set (void);
  set ();
 again:;
  int x = x + 1;
  void __attribute__((noipa)) set (void) { x = 41; }
  if (pass++ == 0)
    goto again;
  return x;
}

int
main (void)
{
  if (scalar () != 42)
    __builtin_abort ();
  if (aggregate () != 84)
    __builtin_abort ();
  if (through_address () != 42)
    __builtin_abort ();
  if (rerun () != 43)
    __builtin_abort ();
  return 0;
}
