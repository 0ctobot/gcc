/* A noipa function has to stay opaque to IPA-PTA when it's called through a
   pointer.  What it returns also has to be treated as a pointer to any nonlocal
   memory, not just what its body returns.  */
/* { dg-do run } */
/* { dg-options "-O2 -fipa-pta" } */

int a, b;

__attribute__ ((noipa)) int
z0 (int *p)
{
  return *p;
}

static int (*fp) (int *) = z0;

__attribute__ ((noinline)) int
indirect_static (void)
{
  int l = 4;
  int r = fp (&l);
  int v = l;

  if (__builtin_constant_p (v))
    __builtin_abort ();
  return r != 4 || v != 4;
}

__attribute__ ((noinline)) int
indirect_arg (int (*f) (int *))
{
  int l = 4;
  int r = f (&l);
  int v = l;

  if (__builtin_constant_p (v))
    __builtin_abort ();
  return r != 4 || v != 4;
}

__attribute__ ((noipa)) int *
z1 (void)
{
  return &a;
}

__attribute__ ((noinline)) int
returned (void)
{
  b = 1;
  int *r = z1 ();
  *r = 2;
  int v = b;

  if (__builtin_constant_p (v))
    __builtin_abort ();
  return v != 1 || a != 2;
}

int
main (void)
{
  return indirect_static () || indirect_arg (z0) || returned ();
}
