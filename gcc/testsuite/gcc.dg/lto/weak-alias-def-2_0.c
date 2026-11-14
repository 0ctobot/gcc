/* { dg-lto-do run } */
/* { dg-require-weak "" } */
/* { dg-require-alias "" } */
/* { dg-lto-options { { -O2 -flto } { -O0 -flto } } } */

/* The aliases of weak definitions stay defined when the definitions that
   override them are in an object that isn't LTO.  */

__attribute__ ((weak)) int w = 1;
extern int a __attribute__ ((alias ("w")));

__attribute__ ((weak)) int
f (void)
{
  return 1;
}

int g (void) __attribute__ ((alias ("f")));

int
main (void)
{
  if (a != 1 || w != 10 || g () != 1 || f () != 10)
    __builtin_abort ();
  return 0;
}
