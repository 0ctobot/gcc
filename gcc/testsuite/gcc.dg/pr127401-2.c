/* PR tree-optimization/127401 */
/* { dg-do compile } */
/* { dg-options "-O2 -fhoist-adjacent-loads -fdump-tree-phiopt2-details -fdump-tree-phiopt4-details" } */

/* Adjacent _Bool fields are still hoisted, with each load reading the whole
   byte.  Sinking moves the loads back into the arms after phiopt2, so phiopt4
   has to recognize them in order to hoist them again.  */

struct S { _Bool a; _Bool b; };

int
f (struct S *s, int c)
{
  int r;
  if (c)
    r = s->a;
  else
    r = s->b;
  return r;
}

/* { dg-final { scan-tree-dump-times "Hoisting adjacent loads" 1 "phiopt2" { target aarch64*-*-* } } } */
/* { dg-final { scan-tree-dump-times "Hoisting adjacent loads" 1 "phiopt4" { target aarch64*-*-* } } } */
