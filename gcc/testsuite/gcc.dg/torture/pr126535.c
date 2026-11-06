/* PR tree-optimization/126535 */
/* { dg-do run } */
/* { dg-additional-options "-lm" } */

/* A true UNEQ_EXPR says nothing about X when Y may be a NaN, so the range
   of Y must not carry over to X.  __builtin_islessgreater is !(x UNEQ y).  */

__attribute__((noipa)) int
uneq (double x, int i)
{
  double a = (double) ((i & 3) - 1);
  double y = __builtin_sqrt (a);

  if (__builtin_islessgreater (x, y))
    return 3;
  if (x > 100.0)
    return 1;
  return 2;
}

/* Likewise for a false LTGT_EXPR, which ifcombine forms here.  */

__attribute__((noipa)) int
ltgt (double x, int i)
{
  double a = (double) ((i & 3) - 1);
  double y = __builtin_sqrt (a);

  if (x < y || x > y)
    return 3;
  if (x > 100.0)
    return 1;
  return 2;
}

int
main (void)
{
  if (uneq (1e9, 0) != 1)
    __builtin_abort ();
  if (ltgt (1e9, 0) != 1)
    __builtin_abort ();
  return 0;
}
