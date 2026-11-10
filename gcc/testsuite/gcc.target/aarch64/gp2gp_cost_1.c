/* { dg-do compile } */
/* { dg-options "-O2 -fno-optimize-sibling-calls -mtune=cortex-a725" } */

/* Cortex-A725 uses the Neoverse N3 tuning.  A GP2GP cost above the cost
   of a store made IRA keep the incoming argument pseudos below in memory,
   so add_all stored p to a new stack slot it never read and f stored b
   on its fast path.  */

void
add_all (long *p, long n, long v)
{
  for (long i = 0; i < n; i++)
    p[i] += v;
}

long ext (long);

long
f (long a, long b, long c)
{
  if ((short) a < 0)
    return ext (b);
  return c;
}

/* { dg-final { scan-assembler-not {\tstr\tx[0-9]+, \[sp} } } */
/* { dg-final { scan-assembler-not {\tsub\tsp, sp} } } */
