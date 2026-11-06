/* PR tree-optimization/127345 */
/* { dg-do run { target bitint } } */
/* { dg-options "-std=c23" } */

struct S { _BitInt (3) c; };

/* SRA replaces S.C with a _BitInt (8), which holds the zero-extended value of
   X here, and reading the member has to discard the upper bits of that.  */

[[gnu::noipa]] long long
f (unsigned _BitInt (3) x, struct S *p, int c)
{
  struct S s;
  if (c)
    s.c = (_BitInt (3)) x;
  else
    s = *p;
  return s.c;
}

int
main ()
{
  struct S t = { 1 };
  if (f (7, &t, 1) != -1)
    __builtin_abort ();
  if (f (7, &t, 0) != 1)
    __builtin_abort ();
}
