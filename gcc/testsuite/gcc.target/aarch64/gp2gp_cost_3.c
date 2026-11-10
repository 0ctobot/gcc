/* { dg-do compile } */
/* { dg-options "-O2 -fno-optimize-sibling-calls -mtune=neoverse-v3ae" } */

/* Same as gp2gp_cost_1.c, for the Neoverse V3AE tuning.  */

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
