/* PR tree-optimization/114206 */
/* A branch on what a callee returned can only tell a tail call which way it
   went if more than one of its successors leads to that call.  An early return
   nested in an arm or a loop leaves just one, so these tail calls stay, while a
   loop that keeps going on the result still decides what the call can see.  */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-tailc-details" } */

struct key { long a; long b; };
struct dev { long a; long b; struct dev *next; };

extern long val;
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
  return kept (dev);
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
  return kept (dev);
}

/* The result only ever ends the loop early.  */
long
loop_early (struct dev *dev, long n)
{
  struct key x;
  for (long i = 0; i < n; i++)
    if (helper (&x, i))
      return -1;
  return kept (dev);
}

long
list_walk_early (struct dev *dev)
{
  struct key x;
  for (struct dev *p = dev; p; p = p->next)
    if (helper (&x, p->a))
      return -1;
  return kept (dev);
}

/* Each successor of the branch leads to a tail call of its own.  */
long
both_calls (struct dev *dev, int flag)
{
  struct key x;
  if (flag)
    {
      if (helper (&x, 0))
	return kept_too (dev);
      foo (dev);
    }
  return kept (dev);
}

/* How many times foo () runs depends on the result.  */
long
retry (struct dev *dev)
{
  struct key x;
  do
    foo (dev);
  while (helper (&x, 0));
  return refused (dev);
}

/* { dg-final { scan-tree-dump-times "Found tail call \[^\n\]*kept" 6 "tailc" } } */
/* { dg-final { scan-tree-dump-not "Found tail call \[^\n\]*refused" "tailc" } } */
