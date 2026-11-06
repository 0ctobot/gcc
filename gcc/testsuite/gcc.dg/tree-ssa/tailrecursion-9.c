/* Getting past the early return on what USE returned can confirm a guess of
   part of the local's address, so even a narrow argument that can't hold all
   of it keeps the recursion from turning into a loop.  */
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

/* { dg-final { scan-tree-dump-not "Eliminated tail recursion" "tailr1" } } */
