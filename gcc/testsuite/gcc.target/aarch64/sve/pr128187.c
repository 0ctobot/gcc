/* { dg-do compile } */
/* { dg-options "-O3 -ffast-math -fno-vect-cost-model" } */

double
f (double *a, int *d, int n, double s)
{
  for (int i = 0; i < n; i++)
    {
      s = __builtin_fma (a[i], a[i], s);
      d[i] = 1;
    }
  return s;
}
