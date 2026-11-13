/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-optimized" } */
/* { dg-final { scan-tree-dump-times "PHI <" 2 "optimized" } } */

/* popcount (0) is 0, so the zero check can be removed from f1 and f2 but
   not from g1 and g2, where the other PHI argument doesn't match.  */

int
f1 (unsigned int x)
{
  return x ? __builtin_popcount (x) * 2 + 1 : 1;
}

long
f2 (unsigned long x)
{
  return x ? (long) __builtin_popcountl (x) - 7 : -7;
}

int
g1 (unsigned int x)
{
  return x ? __builtin_popcount (x) * 2 + 1 : 0;
}

int
g2 (unsigned int x)
{
  return x ? __builtin_popcount (x) - 7 : 7;
}
