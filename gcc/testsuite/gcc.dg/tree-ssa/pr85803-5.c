/* PR tree-optimization/85803 */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-optimized" } */

/* Conditional store replacement moves the stores from both arms of a condition
   down to the block that joins the arms.  That doesn't work when the function
   may never get past something behind the store.  */

int a1, a2;

__attribute__ ((noinline)) static int
spin (int c)
{
  while (c);
  return 0;
}

void
f1 (int c, int d)
{
  if (c)
    {
      a1 = 1;
      spin (d);
    }
  else
    a1 = 2;
}

void
f2 (int c)
{
  if (c)
    {
      a2 = 1;
      __asm__ volatile ("");
    }
  else
    a2 = 2;
}

/* { dg-final { scan-tree-dump-not "cstore" "optimized" } } */
