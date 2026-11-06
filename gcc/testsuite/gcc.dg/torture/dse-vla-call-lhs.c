/* A call returning a variably sized struct stores to its whole LHS, so a later
   load from a constant offset into the LHS depends on the call and DSE mustn't
   delete the call.  */
/* { dg-do run } */
/* { dg-additional-options "-std=gnu23 -w" } */

/* get40 is called through a pointer to a function returning a variably sized
   struct, which is always returned in memory.  Make the struct too big for any
   ABI to return in registers.  */

struct d { int a[40]; };

__attribute__ ((noinline)) struct d
get40 (void)
{
  struct d r = { { [0] = 42, [39] = 46 } };
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
  if (call_indirect (40, get40) != 46)
    __builtin_abort ();
  return 0;
}
