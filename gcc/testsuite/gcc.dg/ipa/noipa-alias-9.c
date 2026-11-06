/* The noipa on b has to reach every alias further along the chain when the
   chain is resolved from its far end, not just the alias right after b.  */
/* { dg-do run } */
/* { dg-require-alias "" } */
/* { dg-options "-O2" } */

extern int d (int) __attribute__ ((alias ("c")));
extern int c (int) __attribute__ ((alias ("b")));
extern int b (int) __attribute__ ((noipa, alias ("a0")));
extern int a0 (int) __attribute__ ((alias ("a")));

int
a (int v)
{
  return v + 1;
}

int
main (void)
{
  int x = d (5);
  int y = c (5);
  int (*volatile fp) (int) = d;

  if (__builtin_constant_p (x) || __builtin_constant_p (y))
    __builtin_abort ();
  return x != 6 || y != 6 || fp (5) != 6;
}
