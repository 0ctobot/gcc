/* The square of a NaN can keep its sign, but the square of anything else is
   nonnegative.  These folds therefore need to know that x isn't a NaN.  */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-optimized" } */

int
f1 (int i)
{
  double x = i;
  return __builtin_signbit (x * x);
}

double
f2 (int i)
{
  double x = i;
  return __builtin_fabs (x * x);
}

int
f3 (double x)
{
  if (x != x)
    return 0;
  return __builtin_signbit (x * x);
}

double
f4 (double x)
{
  if (__builtin_isnan (x))
    return 0;
  return __builtin_fabs (x * x);
}

/* Nothing is known about x here, so the sign must still be computed.  */
int
f5 (double x)
{
  return __builtin_signbit (x * x);
}

double
f6 (double x)
{
  return __builtin_fabs (x * x);
}

/* The check only runs when c is set, so x can still be a NaN at the call.  */
int
f7 (double x, int c)
{
  if (c && x != x)
    return 7;
  return __builtin_signbit (x * x);
}

/* { dg-final { scan-tree-dump-times "__builtin_signbit" 2 "optimized" } } */
/* { dg-final { scan-tree-dump-times "ABS_EXPR" 1 "optimized" } } */
