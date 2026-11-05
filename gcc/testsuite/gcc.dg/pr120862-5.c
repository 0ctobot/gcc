/* { dg-do compile } */
/* { dg-options "-std=gnu23 -fchecking -O2" } */
/* { dg-require-effective-target trampolines } */

/* Taking the sibling's address leaves a reference to it in the inlined body.  */

int
c (int n)
{
  struct d { int a[n]; };
  __attribute__ ((noinline)) struct d e (void)
  {
    struct d f;
    f.a[0] = 42;
    return f;
  }
  void g (void)
  {
    struct d (*volatile fp) (void) = e;
    fp ();
  }
  g ();
  return 0;
}
