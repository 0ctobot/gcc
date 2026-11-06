/* PR tree-optimization/85803 */
/* { dg-do compile } */
/* { dg-options "-O2 -fpredictive-commoning -fno-tree-dse -fdump-tree-pcom-details" } */

/* Predictive commoning removes a store that a later store in the loop kills.
   That doesn't work when the function may never get to the later store.  */

int a[64], b[64];
volatile int v;

__attribute__ ((noinline)) static void
hang (int c)
{
  if (c)
    for (;;);
}

void
f1 (int n)
{
  for (int i = 0; i < n; i++)
    {
      a[i] = i;
      (void) v;
      a[i] = i + 1;
    }
}

void
f2 (int n, int c)
{
  for (int i = 0; i < n; i++)
    {
      b[i] = i;
      hang (c);
      b[i] = i + 1;
    }
}

/* { dg-final { scan-tree-dump-not "Store-stores chain" "pcom" } } */
