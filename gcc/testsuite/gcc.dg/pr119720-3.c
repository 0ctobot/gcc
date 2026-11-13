/* PR tree-optimization/119720 */
/* { dg-do compile } */
/* { dg-options "-O2 -fstrict-aliasing -fhoist-adjacent-loads --param l1-cache-line-size=64 -fdump-tree-phiopt2-details" } */

/* With strict aliasing, reading a field of *p implies that the whole of *p
   exists, so the loads in f () are hoisted.  An access through a may_alias
   type doesn't imply that, so the loads in h () aren't.  */

struct s { int a; int b; };
struct __attribute__((may_alias)) sa { int a; int b; };

int
f (int c, struct s *p)
{
  int t;
  if (c)
    t = p->a;
  else
    t = p->b;
  return t;
}

int
h (int c, struct sa *q)
{
  int t;
  if (c)
    t = q->a;
  else
    t = q->b;
  return t;
}

/* { dg-final { scan-tree-dump-times "Hoisting adjacent loads" 1 "phiopt2" { target { aarch64*-*-* x86_64-*-* i?86-*-* } } } } */
/* { dg-final { scan-tree-dump-not "Hoisting adjacent loads\[^\n\r]*\[\n\r]+(#\[^\n\r]*\[\n\r]+)?\[^\n\r]*q_\[0-9\]+\\(D\\)" "phiopt2" { target { aarch64*-*-* x86_64-*-* i?86-*-* } } } } */
