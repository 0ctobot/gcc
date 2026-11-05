/* PR c/120862 */
/* { dg-do compile } */
/* { dg-options "-std=gnu23 -fchecking -Os" } */

/* Same as pr120862.c for forms that only inline the sibling at -Os.  */

void
c1 (int n)
{
  struct S { int a[n]; };
  struct S fn (void)
  {
    struct S s;
    s.a[0] = 42;
    return s;
  }
  struct S fn2 (void)
  {
    return fn ();
  }
  fn ();
  fn2 ();
}

void
c2 (int n)
{
  struct S { int a[n]; };
  struct S fn (void)
  {
    struct S s;
    s.a[0] = 42;
    return s;
  }
  auto struct S fn2 (void)
  {
    return fn ();
  }
  struct S x;
  fn ();
  fn2 ();
  x = fn ();
  if (x.a[0] != 42)
    __builtin_abort ();
  if (fn ().a[0] != 42)
    __builtin_abort ();
  __typeof__ (fn ()) *p = &x;
  if (p->a[0] != 42)
    __builtin_abort ();
  if (fn2 ().a[0] != 42)
    __builtin_abort ();
}
