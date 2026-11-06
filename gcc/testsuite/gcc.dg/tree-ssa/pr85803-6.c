/* PR tree-optimization/85803 */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-tailc" } */

/* A tail call pops the frame of the caller.  Whoever got the address of a local
   variable can still read it while the callee keeps running, even if the callee
   doesn't touch that variable itself.  */

extern void publish (int *);

__attribute__ ((noinline)) static void
hang (int c)
{
  if (c)
    for (;;);
}

void
f1 (int c)
{
  int l;
  publish (&l);
  l = 42;
  hang (c);
}

/* { dg-final { scan-tree-dump-not "\\\[tail call\\\]" "tailc" } } */
