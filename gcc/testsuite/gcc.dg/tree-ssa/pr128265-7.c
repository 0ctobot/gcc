/* PR tree-optimization/128265 */
/* { dg-do run } */
/* { dg-options "-O1" } */

/* Both x and b1 hold a byte that isn't a valid _Bool while the speculated
   loads read them, which must not change the result.  */

union u { _Bool b; unsigned char c; } x;
_Bool b1, b2;

__attribute__ ((noipa)) int
f (int c, _Bool d)
{
  _Bool *p = c ? &x.b : &b2;
  return d ? *p : 1;
}

__attribute__ ((noipa)) int
g (int c, _Bool d)
{
  _Bool *p = c ? &b1 : &b2;
  return d ? *p : 1;
}

int
main ()
{
  x.c = 2;
  *(unsigned char *) &b1 = 2;
  if (f (1, 0) != 1 || g (1, 0) != 1)
    __builtin_abort ();
  x.b = 1;
  b1 = 1;
  if (f (1, 1) != 1 || g (1, 1) != 1 || f (0, 1) != 0 || g (0, 1) != 0)
    __builtin_abort ();
  return 0;
}
