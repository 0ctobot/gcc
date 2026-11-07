/* PR tree-optimization/128312 */
/* { dg-do run } */
/* { dg-options "" } */

/* A nested function reads a variable of its parent through the static chain
   without the address of the variable being taken, so a call to it in a
   second run of an initializer can read the old value.  That holds for a pure
   nested function too.  */

struct S { long x; long pad; long a[6]; };

__attribute__ ((noipa)) void
pure_call (void)
{
  auto long rd (void) __attribute__ ((pure, noinline));
  int n = 0;
 again:
  ;
  struct S s = { .x = n + 100, .a[3] = n ? rd () + 1 : 7 };
  long __attribute__ ((pure, noinline)) rd (void) { return s.a[3]; }
  if (n++ == 0)
    goto again;
  if (s.a[3] != 8)
    __builtin_abort ();
}

__attribute__ ((noipa)) void
plain_call (void)
{
  auto long rd (void) __attribute__ ((noinline));
  int n = 0;
 again:
  ;
  struct S s = { .x = n + 100, .a[3] = n ? rd () + 1 : 7 };
  long __attribute__ ((noinline)) rd (void) { return s.a[3]; }
  if (n++ == 0)
    goto again;
  if (s.a[3] != 8)
    __builtin_abort ();
}

int
main (void)
{
  pure_call ();
  plain_call ();
  return 0;
}
