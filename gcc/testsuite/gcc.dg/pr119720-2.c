/* PR tree-optimization/119720 */
/* { dg-do compile } */
/* { dg-options "-O2 -fno-strict-aliasing -fhoist-adjacent-loads --param l1-cache-line-size=64 -fdump-tree-phiopt2-details" } */

/* With -fno-strict-aliasing, the loads in f () are still hoisted because g
   is a declared object that holds both fields.  The loads in h () aren't,
   since p might only point to enough memory for the field it reads.  */

struct s { int a; int b; };
struct s g;

int
f (int c)
{
  int t;
  if (c)
    t = g.a;
  else
    t = g.b;
  return t;
}

int
h (int c, struct s *p)
{
  int t;
  if (c)
    t = p->a;
  else
    t = p->b;
  return t;
}

/* { dg-final { scan-tree-dump-times "Hoisting adjacent loads" 1 "phiopt2" { target { aarch64*-*-* x86_64-*-* i?86-*-* } } } } */
/* { dg-final { scan-tree-dump-not "Hoisting adjacent loads\[^\n\r]*\[\n\r]+(#\[^\n\r]*\[\n\r]+)?\[^\n\r]*p_\[0-9\]+\\(D\\)->a" "phiopt2" { target { aarch64*-*-* x86_64-*-* i?86-*-* } } } } */
