/* A call returning a variably sized struct stores to its whole LHS, so a later
   load from a constant offset into the LHS depends on the call and DSE mustn't
   delete the call.  */
/* { dg-do run } */
/* { dg-additional-options "-std=gnu23 -w" } */

struct d { int a[5]; };

__attribute__ ((noinline)) struct d
get5 (void)
{
  struct d r = { { 42, 43, 44, 45, 46 } };
  return r;
}

__attribute__ ((noinline)) int
nested (int n)
{
  struct d { int a[n]; };
  __attribute__ ((noinline, pure)) struct d
  e (void)
  {
    struct d f;
    f.a[0] = 42;
    return f;
  }
  struct d y;
  y = e ();
  return y.a[0];
}

static inline __attribute__ ((always_inline)) int
indirect (int n, struct d { int a[n]; } (*fp) (void))
{
  struct d y;
  y = fp ();
  return y.a[n - 1];
}

__attribute__ ((noinline)) int
call_indirect (int n, void *v)
{
  return indirect (n, v);
}

int
main (void)
{
  if (nested (5) != 42)
    __builtin_abort ();
  if (call_indirect (5, get5) != 46)
    __builtin_abort ();
  return 0;
}
