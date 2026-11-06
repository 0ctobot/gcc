/* PR tree-optimization/114206 */
/* Tail recursion can only merge the copies of a forwarded aggregate parameter
   if its address can't reach the next activation.  */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-tailr1-details" } */

struct b { long v[4]; };
__attribute__ ((pure)) extern int use (struct b *);

long
refused (struct b a, int n, struct b *prev)
{
  a.v[0] = n;
  long seen = prev ? prev->v[0] : 0;
  if (n == 0)
    return seen;
  return refused (a, n - 1, &a);
}

long
kept (struct b a, int n)
{
  if (use (&a))
    return 1;
  if (n == 0)
    return 0;
  return kept (a, n - 1);
}

/* { dg-final { scan-tree-dump-times "Eliminated tail recursion" 1 "tailr1" } } */
