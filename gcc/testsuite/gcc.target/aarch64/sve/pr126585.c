/* { dg-do compile } */
/* { dg-options "-O3 -march=armv8.2-a+sve -msve-vector-bits=256" } */

/* The COND_EXPRs need two vectors per iteration, while the store to B on the
   same condition records a loop mask for a single vector.  This used to
   segfault looking up the missing two-vector mask.  */

void
f (int *restrict p, signed char *restrict b, int *restrict o, int n)
{
  for (int i = 0; i < n; i++)
    {
      signed char t0 = p[2 * i] >> 3;
      signed char t1 = p[2 * i + 1] >> 3;
      if (t0 > 0)
	b[i] = 1;
      o[2 * i] = (t0 > 0 ? t0 : 3) * p[2 * i];
      o[2 * i + 1] = (t1 > 0 ? t1 : 3) * p[2 * i + 1];
    }
}
