/* { dg-lto-do run } */
/* { dg-lto-options { { -O2 -flto } } } */

/* A call through a noipa declaration must not see the body of a definition
   in another TU, even though the definition's own declaration doesn't have
   the attribute. Inlining the body would make the result here a constant.  */

extern void abort (void);

__attribute__ ((noipa)) int z (int);

int
main (void)
{
  int x = z (5);
  if (__builtin_constant_p (x))
    abort ();
  return x != 6;
}
