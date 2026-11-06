/* c is declared before its noipa target b, so c is analyzed first.  Resolving
   b afterwards re-points c directly onto a, which mustn't lose the noipa on
   b.  */
/* { dg-do run } */
/* { dg-require-alias "" } */
/* { dg-options "-O2" } */

extern int c (int) __attribute__ ((alias ("b")));
extern int b (int) __attribute__ ((noipa, alias ("a")));

int
a (int v)
{
  return v + 1;
}

int
main (void)
{
  int x = c (5);

  if (__builtin_constant_p (x))
    __builtin_abort ();
  return x != 6;
}
