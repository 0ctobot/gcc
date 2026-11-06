/* A call of a noipa function clobbers global memory even when nothing is passed
   and the body doesn't touch it, for its direct callers and for their callers
   too.  What the body really does still counts for its callers.  */
/* { dg-do run } */
/* { dg-options "-O2 -fipa-pta" } */

int g;

__attribute__ ((noipa)) void
nop (void)
{
}

__attribute__ ((noinline)) int
direct (void)
{
  g = 1;
  nop ();
  int v = g;

  if (__builtin_constant_p (v))
    __builtin_abort ();
  return v != 1;
}

__attribute__ ((noinline)) void
wrapper (void)
{
  nop ();
}

__attribute__ ((noinline)) int
nested (void)
{
  g = 1;
  wrapper ();
  int v = g;

  if (__builtin_constant_p (v))
    __builtin_abort ();
  return v != 1;
}

static int *gp;

__attribute__ ((noipa)) void
store (void)
{
  *gp = 5;
}

__attribute__ ((noinline)) int
through_static (void)
{
  int l = 1;

  gp = &l;
  store ();
  return l != 5;
}

int
main (void)
{
  return direct () || nested () || through_static ();
}
