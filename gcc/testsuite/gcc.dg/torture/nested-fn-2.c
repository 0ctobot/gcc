/* { dg-do run } */

/* A nested function reads the variables of its parent through the static
   chain.  When the parent assigns the result of a call to the nested function
   to one of those variables, the variable can't be the return slot of the
   call, since the nested function reads its old value after it has started to
   store the new one.  */

struct S { long a[8]; };

__attribute__ ((noipa)) void
opaque (void)
{
}

__attribute__ ((noipa)) long
local (void)
{
  struct S v;
  v.a[0] = 5;
  __attribute__ ((noinline)) struct S
  mk (void)
  {
    struct S r;
    for (int i = 0; i < 8; i++)
      r.a[i] = 100 + i;
    opaque ();
    r.a[7] = v.a[0];
    return r;
  }
  v = mk ();
  return v.a[7];
}

__attribute__ ((noipa)) long
param (struct S v)
{
  __attribute__ ((noinline)) struct S
  mk (void)
  {
    struct S r;
    for (int i = 0; i < 8; i++)
      r.a[i] = 100 + i;
    opaque ();
    r.a[7] = v.a[0];
    return r;
  }
  v = mk ();
  return v.a[7];
}

int
main (void)
{
  struct S s = { { 5 } };
  if (local () != 5)
    __builtin_abort ();
  if (param (s) != 5)
    __builtin_abort ();
  return 0;
}
