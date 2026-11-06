/* PR tree-optimization/114206 */
/* What a callee learned about the address of a local can only reach a tail call
   through something that depends on the result of that callee, or through a
   guess of the address that a branch on the result confirmed.  An early return
   on the result doesn't make anything after it depend on the result, so the
   tail calls after it stay as long as they can't be handed such a guess.  */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-tailc-details" } */

struct key { long a; long b; };
struct dev { long a; long b; };

extern long val;
extern struct dev global_dev;
extern long kept (struct dev *);
extern long kept_void (void);
extern long refused (long);
extern long refused_dev (struct dev *);
extern long refused2 (struct dev *, long);
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
  return kept (&global_dev);
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
  return kept (&global_dev);
}

long
early_return_null (struct dev *dev)
{
  struct key x;
  if (!lookup (&x))
    return -2;
  return kept (&global_dev);
}

long
early_return_void (void)
{
  struct key x;
  if (helper (&x))
    return -14;
  return kept_void ();
}

/* The helper could have compared the address against DEV.  */
long
early_return_guess (struct dev *dev)
{
  struct key x;
  if (helper (&x))
    return -14;
  return refused_dev (dev);
}

/* The branch on FLAG doesn't depend on the result of the helper, but the
   arguments could still be a guess.  */
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
  return refused2 (dev, y);
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
