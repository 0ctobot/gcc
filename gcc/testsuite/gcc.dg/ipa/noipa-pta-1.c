/* IPA-PTA looks at the body of a function to work out what a call of it uses,
   clobbers and returns.  It can't do that for a noipa function, so the read of
   l after the call has to stay.  */
/* { dg-do run } */
/* { dg-options "-O2 -fipa-pta" } */

__attribute__ ((noipa)) int
z0 (int *p)
{
  return *p;
}

int
main (void)
{
  int l = 4;
  int r = z0 (&l);
  int v = l;

  if (__builtin_constant_p (v))
    __builtin_abort ();
  return r != 4 || v != 4;
}
