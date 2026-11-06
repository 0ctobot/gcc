/* PR tree-optimization/114206 */
/* Tail recursion can only merge the copies of a forwarded aggregate parameter
   if its address can't reach the next activation, either directly or as a
   guess that a branch on a callee's result confirmed.  */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-tailr1-details" } */

struct b { long v[4]; };
__attribute__ ((pure)) extern int use (struct b *);
extern long sink;

static __attribute__ ((noinline, noclone)) void
look (struct b *p)
{
  sink = p->v[1];
}

long
refused (struct b a, int n, struct b *prev)
{
  a.v[0] = n;
  long seen = prev ? prev->v[0] : 0;
  if (n == 0)
    return seen;
  return refused (a, n - 1, &a);
}

/* Getting past the early return on the result can confirm that N - 1 was a
   guess of A's address, which the next activation would then hold.  */
long
early_return (struct b a, int n)
{
  if (use (&a))
    return 1;
  if (n == 0)
    return 0;
  return early_return (a, n - 1);
}

/* LOOK neither keeps A's address nor returns anything to branch on.  */
long
kept (struct b a, int n)
{
  look (&a);
  if (n == 0)
    return 0;
  return kept (a, n - 1);
}

/* { dg-final { scan-tree-dump-times "Eliminated tail recursion" 1 "tailr1" } } */
/* { dg-final { scan-tree-dump {Eliminated tail recursion [^\n]*kept} "tailr1" } } */
