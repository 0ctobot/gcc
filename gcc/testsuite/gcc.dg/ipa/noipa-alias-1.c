/* A call through an alias of a noipa function must not be optimized using
   properties discovered from the body of that function.  */
/* { dg-do run } */
/* { dg-require-alias "" } */
/* { dg-options "-O2" } */

__attribute__ ((noipa)) int
a (int v)
{
  return v;
}

extern int b (int) __attribute__ ((alias ("a")));

int
main ()
{
  int c = b (1) == b (1);
  if (__builtin_constant_p (c))
    __builtin_abort ();
  return 0;
}
