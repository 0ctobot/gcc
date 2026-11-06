/* { dg-do run } */
/* { dg-options "-O -fgimple -w -Wno-psabi" } */

#define N 16

typedef int V __attribute__((vector_size(N * sizeof (int))));
typedef V M __attribute__((vector_mask));

M gx, gy, ge, gn;

void __attribute__((noipa)) __GIMPLE (ssa, startwith("veclower2"))
f (void)
{
  M x;
  M y;
  M e;
  M n;

  __BB(2):
  x_1 = gx;
  y_2 = gy;
  e_3 = x_1 == y_2;
  n_4 = x_1 != y_2;
  ge = e_3;
  gn = n_4;
  return;
}

int
main (void)
{
  int x[N], y[N], e[N], n[N];

  /* Masks with one bit per element, as on x86 with AVX-512, have a different
     layout that the checks below don't decode.  */
  if (sizeof (M) != sizeof (V))
    return 0;

  for (int i = 0; i < N; i++)
    {
      x[i] = i & 1 ? -1 : 0;
      y[i] = i & 2 ? -1 : 0;
    }

  __builtin_memcpy (&gx, x, sizeof (x));
  __builtin_memcpy (&gy, y, sizeof (y));

  f ();

  __builtin_memcpy (e, &ge, sizeof (e));
  __builtin_memcpy (n, &gn, sizeof (n));
  for (int i = 0; i < N; i++)
    if (e[i] != (x[i] == y[i] ? -1 : 0) || n[i] != (x[i] != y[i] ? -1 : 0))
      __builtin_abort ();

  return 0;
}
