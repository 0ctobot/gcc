/* PR c/120862 */
/* { dg-do compile } */
/* { dg-options "-std=gnu23 -fchecking -O2" } */

/* Nested functions using a variably modified type whose size is a variable in
   their parent.  Inlining one of them into the parent used to copy the type,
   leaving the copy to mismatch the signatures of the siblings.  */

int b;

void
c1 ()
{
  struct d
  {
    int a[b];
  } __attribute__ ((aligned)) e ()
  {
    struct d f;
    return f;
  }
  struct d g () { return e (); }
  g ();
}

void
c2 (int n)
{
  struct S { int a[n]; int a_1[n]; };
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
  if (x.a[0] != 42 || fn ().a[0] != 42 || fn2 ().a[0] != 42)
    __builtin_abort ();
}

/* The caller of the sibling stores the result in a local.  */

int
c3 (int n)
{
  struct d { int a[n]; };
  __attribute__ ((noinline)) struct d e (void)
  {
    struct d f;
    f.a[0] = 42;
    return f;
  }
  int g (void)
  {
    struct d y;
    y = e ();
    return y.a[0];
  }
  return g () + e ().a[0];
}

/* A chain of siblings returning the type.  */

int
c4 (int n)
{
  struct d { int a[n]; };
  __attribute__ ((noinline)) struct d e (void)
  {
    struct d f;
    f.a[0] = 42;
    return f;
  }
  struct d g (void) { return e (); }
  struct d h (void) { return g (); }
  return h ().a[0];
}

/* Constant propagation clones the nested function.  */

int
c5 (int n)
{
  struct d { int a[n]; };
  __attribute__ ((noinline)) struct d e (int x)
  {
    struct d f;
    f.a[0] = x;
    return f;
  }
  return e (42).a[0] + e (42).a[0] + e (42).a[0];
}
