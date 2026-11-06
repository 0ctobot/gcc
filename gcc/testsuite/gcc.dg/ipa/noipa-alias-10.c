/* Calls through the plain alias c stay opaque after resolve_alias re-points it,
   while calls of the plain body a right next to them are still inlined.  */
/* { dg-do compile } */
/* { dg-require-alias "" } */
/* { dg-options "-O2 -fdump-tree-optimized" } */

extern int c (int) __attribute__ ((alias ("b")));
extern int b (int) __attribute__ ((noipa, alias ("a")));

int
a (int v)
{
  return v + 1;
}

int
call_c (void)
{
  return c (5);
}

int
call_a (void)
{
  return a (5);
}

/* { dg-final { scan-tree-dump-times "c \\(5\\)" 1 "optimized" } } */
/* { dg-final { scan-tree-dump-not "a \\(5\\)" "optimized" } } */
