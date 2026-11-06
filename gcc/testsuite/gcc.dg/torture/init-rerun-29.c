/* { dg-do run } */
/* { dg-require-effective-target nonlocal_goto } */
/* { dg-options "-ftrivial-auto-var-init=zero" } */

/* A goto out of a nested function back to a label above a declaration in the
   same block doesn't end the lifetime of the variable, just like a goto in the
   function itself.  A re-run of the initializer has to see the value of the
   previous run rather than the automatic initialization, and an aggregate
   whose initializer reads its old value through its address mustn't be
   cleared or built in place.  */

struct S { int a[4]; int k; };

__attribute__ ((noipa)) int
get (struct S *p)
{
  return p->a[0] * 10 + p->k;
}

__attribute__ ((noipa)) int
scalar (void)
{
  __label__ again;
  volatile int runs = 0;
  __attribute__ ((noipa)) void back (void) { goto again; }
 again:;
  int x = runs ? x * 10 + 1 : 1;
  if (runs++ < 2)
    back ();
  return x;
}

__attribute__ ((noipa)) int
aggregate (void)
{
  __label__ again;
  volatile int runs = 0;
  __attribute__ ((noipa)) void back (void) { goto again; }
 again:;
  struct S v = { .k = 1, .a[0] = runs ? get (&v) : 1 };
  if (runs++ < 2)
    back ();
  return v.a[0] * 10 + v.k;
}

int
main (void)
{
  if (scalar () != 111)
    __builtin_abort ();
  if (aggregate () != 1111)
    __builtin_abort ();
  return 0;
}
