/* A noipa function's definition says nothing about what its callers can rely
   on, including which arguments it takes.  A pointer passed in an argument that
   the definition doesn't name still escapes.  */
/* { dg-do run } */
/* { dg-options "-O2 -fipa-pta -std=gnu11 -w" } */

__attribute__ ((noipa)) int
no_params (void)
{
  return 4;
}

__attribute__ ((noipa)) int
no_prototype ()
{
  return 4;
}

__attribute__ ((noinline)) int
direct_cast (void)
{
  int l = 4;
  int r = ((int (*) (int *)) no_params) (&l);
  int v = l;

  if (__builtin_constant_p (v))
    __builtin_abort ();
  return r != 4 || v != 4;
}

__attribute__ ((noinline)) int
unprototyped (void)
{
  int l = 4;
  int r = no_prototype (&l);
  int v = l;

  if (__builtin_constant_p (v))
    __builtin_abort ();
  return r != 4 || v != 4;
}

static int (*fp) (int *) = (int (*) (int *)) no_params;

__attribute__ ((noinline)) int
indirect (void)
{
  int l = 4;
  int r = fp (&l);
  int v = l;

  if (__builtin_constant_p (v))
    __builtin_abort ();
  return r != 4 || v != 4;
}

int
main (void)
{
  return direct_cast () || unprototyped () || indirect ();
}
