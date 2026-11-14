/* { dg-lto-do run } */
/* { dg-require-weak "" } */
/* { dg-require-alias "" } */
/* { dg-lto-options { { -O2 -flto } { -O0 -flto } } } */

/* Without an overriding definition, a weak definition and its alias have the
   same address.  */

__attribute__ ((weak)) int w = 1;
extern int a __attribute__ ((alias ("w")));

__attribute__ ((weak)) int
f (void)
{
  return 1;
}

int g (void) __attribute__ ((alias ("f")));

__attribute__ ((noipa)) int *
addr_w (void)
{
  return &w;
}

__attribute__ ((noipa)) int *
addr_a (void)
{
  return &a;
}

__attribute__ ((noipa)) void *
addr_f (void)
{
  return (void *) f;
}

__attribute__ ((noipa)) void *
addr_g (void)
{
  return (void *) g;
}

int
main (void)
{
  if (addr_w () != addr_a () || addr_f () != addr_g ())
    __builtin_abort ();
  a = 5;
  if (w != 5 || g () != 1 || f () != 1)
    __builtin_abort ();
  return 0;
}
