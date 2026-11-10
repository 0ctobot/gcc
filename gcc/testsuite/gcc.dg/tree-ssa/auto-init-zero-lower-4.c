/* With -ftrivial-auto-var-init=zero, x reads 0 when c is zero.  Once the
   zero-init marker is replaced by 0, both comparisons fold and the
   "return 3" arm goes away.  */
/* { dg-do compile } */
/* { dg-options "-O2 -ftrivial-auto-var-init=zero -fdump-tree-optimized" } */

int
f (int c)
{
  int x;
  if (c)
    x = 7;
  if (x == 7)
    return 1;
  if (x == 0)
    return 2;
  return 3;
}

/* { dg-final { scan-tree-dump-not "\\.DEFERRED_INIT" "optimized" } } */
/* { dg-final { scan-tree-dump-not "== 7" "optimized" } } */
/* The value 3 reaches the return neither directly nor as a PHI argument.  */
/* { dg-final { scan-tree-dump-not "return 3;|PHI <\[^>\]*\\m3\\(\[0-9\]+\\)" "optimized" } } */
