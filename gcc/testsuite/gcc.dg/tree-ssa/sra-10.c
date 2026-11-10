/* { dg-do compile } */
/* { dg-options "-O1 -fdump-tree-optimized" } */

struct S
{
  int a[1];
  int z[256];
};

void foo (struct S *s, int i)
{
  struct S disappear;

  disappear.a[i] = 12;
  *s = disappear;
}

/* A variable index into a [1] member array may reach the members after it,
   so the store can land anywhere in disappear past a and SRA can't replace
   it with a scalar for a[0].  The store and the copy must both stay.  */
/* { dg-final { scan-tree-dump-times "disappear\\.a\\\[i_\[0-9\]+\\(D\\)\\\] = 12;" 1 "optimized" } } */
/* { dg-final { scan-tree-dump-times "\\*s_\[0-9\]+\\(D\\) = disappear;" 1 "optimized" } } */
