/* PR tree-optimization/85803 */
/* { dg-do compile } */
/* { dg-require-effective-target vect_int } */

/* Vectorizing a group of stores moves all of them down to the last one.  That
   doesn't work when the function may never get past something in between.  */

int a[2], b[2], c[2];
volatile int v;

__attribute__ ((noinline)) static void
hang (int d)
{
  if (d)
    for (;;);
}

void
f1 (int d)
{
  a[0] = 1;
  hang (d);
  a[1] = 2;
}

void
f2 (void)
{
  b[0] = 1;
  __asm__ volatile ("");
  b[1] = 2;
}

void
f3 (void)
{
  c[0] = 1;
  (void) v;
  c[1] = 2;
}

/* { dg-final { scan-tree-dump-not "optimized: basic block" "slp2" } } */
