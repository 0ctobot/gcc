/* PR tree-optimization/114206 */
/* A branch on what a callee returned can only pick the values a tail call
   gets if more than one of its successors leads to that call.  An early
   return nested in an arm or a loop leaves just one, so these tail calls
   stay, while a loop that keeps going on the result still decides what the
   call can see.  Getting past the branch still tells which way it went, which
   can confirm a guess of the address, so the calls only get a constant.  */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-tailr1-details -fdump-tree-tailc-details" } */

struct key { long a; long b; };
struct dev { long a; long b; struct dev *next; };
struct pack { long v[4]; };

extern long val;
extern long sink;
extern struct dev global_dev;
extern long kept (struct dev *);
extern long kept_too (struct dev *);
extern long refused (struct dev *);
extern void foo (struct dev *);

__attribute__ ((noinline, noclone)) int
helper (struct key *k, long i)
{
  k->b = i;
  return val != k->a;
}

__attribute__ ((noinline, noclone)) int
check (struct pack *p, long i)
{
  sink = i;
  return p->v[1] < 0;
}

/* Only one arm of the outer branch calls the helper.  */
long
arm_join (struct dev *dev, int flag)
{
  struct key x;
  if (flag)
    {
      if (helper (&x, 0))
	return -1;
      foo (dev);
    }
  return kept (&global_dev);
}

/* Both arms call the helper and store something before joining.  */
long
arm_join_store (struct dev *dev, int flag)
{
  struct key x;
  if (flag)
    {
      if (helper (&x, 0))
	return -1;
      dev->a = 1;
    }
  else
    {
      if (helper (&x, 1))
	return -2;
      dev->b = 1;
    }
  return kept (&global_dev);
}

/* The result only ever ends the loop early.  */
long
loop_early (struct dev *dev, long n)
{
  struct key x;
  for (long i = 0; i < n; i++)
    if (helper (&x, i))
      return -1;
  return kept (&global_dev);
}

long
list_walk_early (struct dev *dev)
{
  struct key x;
  for (struct dev *p = dev; p; p = p->next)
    if (helper (&x, p->a))
      return -1;
  return kept (&global_dev);
}

/* Each successor of the branch leads to a tail call of its own.  */
long
both_calls (struct dev *dev, int flag)
{
  struct key x;
  if (flag)
    {
      if (helper (&x, 0))
	return kept_too (&global_dev);
      foo (dev);
    }
  return kept (&global_dev);
}

/* How many times foo () runs depends on the result.  */
long
retry (struct dev *dev)
{
  struct key x;
  do
    foo (dev);
  while (helper (&x, 0));
  return refused (&global_dev);
}

/* The parameter forwarded to the next activation only goes to the check, but
   getting past the check can confirm a guess of its address, so neither
   recursion can become a loop.  */
long
recurse_arm (struct pack p, long n, int flag)
{
  if (flag)
    {
      if (check (&p, n))
	return -1;
      sink++;
    }
  if (!n)
    return p.v[0];
  return recurse_arm (p, n - 1, flag);
}

long
recurse_loop (struct pack p, long n)
{
  for (long i = 0; i < 3; i++)
    if (check (&p, i))
      return -1;
  if (!n)
    return p.v[0];
  return recurse_loop (p, n - 1);
}

/* { dg-final { scan-tree-dump-not "Eliminated tail recursion" "tailr1" } } */
/* { dg-final { scan-tree-dump-times "Found tail call \[^\n\]*kept" 6 "tailc" } } */
/* { dg-final { scan-tree-dump-not "Found tail call \[^\n\]*refused" "tailc" } } */
