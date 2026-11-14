/* Points-to analysis must give a variable alias the points-to UID of its
   target without changing the alias's own DECL_UID.  */
/* { dg-do compile } */
/* { dg-require-alias "" } */
/* { dg-options "-O2 -fdump-tree-optimized-uid" } */

int a = 1;
extern int b __attribute__ ((alias ("a")));

int
f (int *p)
{
  b = 1;
  *p = 2;
  return a;
}

/* { dg-final { scan-tree-dump "bD\\.\[0-9\]+ = 1;" "optimized" } } */
/* { dg-final { scan-tree-dump "= aD\\.\[0-9\]+;" "optimized" } } */
/* { dg-final { scan-tree-dump-not "bD\\.(\[0-9\]+) = 1;.*= aD\\.\\1;" "optimized" } } */
