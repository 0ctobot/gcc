/* A large PIC call through the PLT sets R15 to the GOT address after the
   arguments are in place, which used to clobber the fourth argument of a
   preserve_none function.  */
/* { dg-do run { target { lp64 && fpic } } } */
/* { dg-options "-O2 -fPIC -mcmodel=large" } */
/* { dg-skip-if "PR90698" { *-*-darwin* } } */

__attribute__ ((noipa, preserve_none)) long
pn (long a, long b, long c, long d, long e, long f, long g)
{
  return a + b * 2 + c * 3 + d * 4 + e * 5 + f * 6 + g * 7;
}

int
main (void)
{
  if (pn (1, 2, 3, 4, 5, 6, 7) != 140)
    __builtin_abort ();
  return 0;
}
