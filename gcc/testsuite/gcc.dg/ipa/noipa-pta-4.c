/* Another definition can replace a weak function at link time, so IPA-PTA can't
   use the body it sees here to decide what a call of it clobbers.  */
/* { dg-do run } */
/* { dg-require-weak "" } */
/* { dg-options "-O2 -fipa-pta" } */

__attribute__ ((weak, noinline)) int
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
