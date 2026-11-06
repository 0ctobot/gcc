/* PR tree-optimization/114206 */
/* What a callee learned about the address of a local can only reach a tail call
   through something that depends on the result of that callee.  An early return
   on the result doesn't make anything after it depend on the result, so those
   tail calls stay.  */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-tailc-details" } */

struct key { long a; long b; };
struct dev { long a; long b; };

extern long val;
extern struct dev global_dev;
extern long kept (struct dev *);
extern long kept2 (struct dev *, long);
extern long refused (long);
extern long refused_void (void);

__attribute__ ((noinline, noclone)) int
helper (struct key *k)
{
  k->b = 1;
  return val != k->a;
}

__attribute__ ((noinline, noclone)) struct dev *
lookup (struct key *k)
{
  k->b = 1;
  return val == k->a ? &global_dev : 0;
}

long
early_return (struct dev *dev)
{
  struct key x;
  if (helper (&x))
    return -14;
  return kept (dev);
}

long
early_return_with_store (struct dev *dev)
{
  struct key x;
  if (helper (&x))
    {
      dev->a = 1;
      return -14;
    }
  return kept (dev);
}

long
early_return_null (struct dev *dev)
{
  struct key x;
  if (!lookup (&x))
    return -2;
  return kept (dev);
}

/* The branch on FLAG doesn't depend on the result of the helper.  */
long
unrelated_branch (struct dev *dev, int flag, long a, long b)
{
  struct key x;
  long y;
  if (helper (&x))
    return -14;
  if (flag)
    y = a;
  else
    y = b;
  return kept2 (dev, y);
}

long
selected_by_branch (long a, long b)
{
  struct key x;
  long y = helper (&x) ? a : b;
  return refused (y);
}

long
selected_in_memory (long a, long b)
{
  struct key x;
  if (helper (&x))
    val = a;
  else
    val = b;
  return refused (val);
}

long
pointer_as_argument (void)
{
  struct key x;
  return refused ((long) lookup (&x));
}

long
loaded_through_pointer (void)
{
  struct key x;
  struct dev *p = lookup (&x);
  return refused (p->b);
}

long
read_by_callee (long a, long b)
{
  struct key x;
  if (helper (&x))
    val = a;
  else
    val = b;
  return refused_void ();
}

/* A search loop picks the argument by continuing or not.  */
long
search_loop (long start)
{
  struct key x;
  for (long c = start; c < start + 4096; c += 8)
    if (helper (&x))
      return refused (c);
  return -1;
}

/* { dg-final { scan-tree-dump-times "Found tail call \[^\n\]*kept" 4 "tailc" } } */
/* { dg-final { scan-tree-dump-not "Found tail call \[^\n\]*refused" "tailc" } } */
