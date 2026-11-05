/* PR c/120862 */
/* { dg-do compile } */
/* { dg-options "-std=gnu23 -fchecking -O2 -g" } */

/* Passes without the fix.  It covers the types the language hook has to walk to
   tell whether a type depends on a variable in a given function: types that
   contain themselves through a pointer, arrays without a domain, unions and
   anonymous members.  */

int
c1 (int n)
{
  struct s { struct s *next; int (*p)[n]; };
  struct f { int k; int (*q[])[n]; };
  union u { int (*p)[n]; struct { int (*r[2])[n]; }; };
  struct s x = { 0, 0 };
  int (*(*a)[])[n] = 0;
  union u y;
  y.p = 0;
  struct f *z = 0;
  __attribute__ ((noinline)) struct s e (void) { return x; }
  __attribute__ ((noinline)) int g (void)
  {
    struct s t;
    t = e ();
    return t.p == 0 && y.p == 0 && z == 0 && a == 0;
  }
  return g ();
}

static inline __attribute__ ((always_inline)) int
f2 (int n)
{
  struct s { struct s *next; int (*p)[n]; };
  struct f { int k; int (*q[])[n]; };
  union u { int (*p)[n]; struct { int (*r[2])[n]; }; };
  int (*(*a)[])[n] = 0;
  struct s x = { 0, 0 };
  union u y;
  y.p = 0;
  struct f *z = 0;
  return x.p == 0 && y.p == 0 && z == 0 && a == 0;
}

int h2 (int m) { return f2 (m) + f2 (m + 1); }

int
c3 (int n)
{
  int (*(*a)[])[n] = 0;
  int g (void)
  {
    int (*(*b)[])[n] = a;
    return b == 0;
  }
  return g () + g ();
}
