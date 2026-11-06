/* A narrow argument that can't hold the address of a local doesn't get in the
   way of turning the recursion into a loop.  */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-tailr1-details" } */

extern int use (int *) __attribute__ ((pure));

int
f (int n, int sum)
{
  int local = n;
  if (use (&local))
    return sum;
  if (n == 0)
    return sum;
  return f (n - 1, (sum * 3) ^ (n >> 2));
}

/* { dg-final { scan-tree-dump "Eliminated tail recursion" "tailr1" } } */
