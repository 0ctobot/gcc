/* { dg-lto-do run } */
/* { dg-require-weak "" } */
/* { dg-require-alias "" } */
/* { dg-lto-options { { -O2 -flto } { -O0 -flto } } } */

/* An alias of a weak alias names what the weak alias names in this file,
   whatever the order of their declarations and however many weak aliases are
   in between, so it doesn't follow a definition in another object that
   overrides one of them.  */

__attribute__ ((weak)) int w = 1;
extern int u1 __attribute__ ((weak, alias ("w")));
extern int v1 __attribute__ ((alias ("u1")));
extern int v2 __attribute__ ((alias ("u2")));
extern int u2 __attribute__ ((weak, alias ("w")));
extern int u3 __attribute__ ((weak, alias ("w")));
extern int u4 __attribute__ ((weak, alias ("u3")));
extern int v3 __attribute__ ((alias ("u4")));

__attribute__ ((weak)) int
f (void)
{
  return 1;
}

int g1 (void) __attribute__ ((weak, noipa, alias ("f")));
int h1 (void) __attribute__ ((alias ("g1")));
int h2 (void) __attribute__ ((alias ("g2")));
int g2 (void) __attribute__ ((weak, alias ("f")));

int
main (void)
{
  if (v1 != 1 || v2 != 1 || v3 != 1)
    __builtin_abort ();
  if (w != 10 || u1 != 11 || u2 != 12 || u3 != 13 || u4 != 14)
    __builtin_abort ();
  if (h1 () != 1 || h2 () != 1)
    __builtin_abort ();
  if (f () != 10 || g1 () != 11 || g2 () != 12)
    __builtin_abort ();
  return 0;
}
