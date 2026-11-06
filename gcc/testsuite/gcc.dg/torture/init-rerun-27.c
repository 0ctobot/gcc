/* { dg-do run } */
/* { dg-options "-ftrivial-auto-var-init=zero" } */

/* A nested function that refers to a variable can store to it before control
   reaches the declaration of the variable.  The first run of an initializer
   that reads the variable has to see that value, so the variable doesn't get
   the automatic initialization ahead of the initializer.  A re-run after a
   goto back sees the value of the previous run as usual.  */

struct S { int a, b; };

__attribute__ ((noipa)) int
nested_store (void)
{
  volatile int pass = 0;
  auto void set (void);
  set ();
 again:;
  struct S s = { s.a + 1, 2 };
  __attribute__ ((noipa)) void set (void) { s.a = 41; }
  if (pass++ == 0)
    goto again;
  return s.a;
}

int
main (void)
{
  if (nested_store () != 43)
    __builtin_abort ();
  return 0;
}
