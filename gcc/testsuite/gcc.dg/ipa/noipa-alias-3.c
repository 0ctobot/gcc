/* The properties must not reach a caller one level further out either.  */
/* { dg-do run } */
/* { dg-require-alias "" } */
/* { dg-options "-O2" } */

__attribute__ ((noipa)) int
a (int v)
{
  return v;
}

extern int b (int) __attribute__ ((alias ("a")));

__attribute__ ((noinline)) int
c (int v)
{
  return b (v);
}

int
main ()
{
  int d = c (1) == c (1);
  if (__builtin_constant_p (d))
    __builtin_abort ();
  return 0;
}
