/* PR rtl-optimization/128570 */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-rtl-loop2_invariant" } */

/* An asm without a "memory" clobber is still hoisted out of the loop, and
   one with a "memory" clobber stays in it.  */

long
hoist (long *p, int n)
{
  long s = 0, v;
  for (int i = 0; i < n; i++)
    {
      __asm__ ("mrs %0, tpidr_el1" : "=r" (v));
      s += v;
      p[i] = s;
    }
  return s;
}

long
keep (long *p, long *q, int n)
{
  long s = 0, v;
  for (int i = 0; i < n; i++)
    {
      __asm__ ("ldr %0, [%1]" : "=r" (v) : "r" (q) : "memory");
      s += v;
      p[i] = s;
    }
  return s;
}

/* { dg-final { scan-rtl-dump-times "Decided to move invariant" 1 "loop2_invariant" } } */
