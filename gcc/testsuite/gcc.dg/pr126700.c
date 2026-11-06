/* { dg-do run } */
/* { dg-options "-O -fgimple -w -Wno-psabi" } */

/* Block 3 uses the comparisons defined in block 4, so vector lowering reaches
   the VEC_COND_EXPR before it reaches the comparisons.  */

#define N 16

typedef int V __attribute__((vector_size(N * sizeof (int))));
typedef V M __attribute__((vector_mask));

M gr, gz;
V ga, gb, gc;

void __attribute__((noipa)) __GIMPLE (ssa, startwith("veclower2"))
f (void)
{
  V a;
  V b;
  V c;
  M m;
  M n;
  M o;
  M t;

  __BB(2):
  a_1 = ga;
  b_2 = gb;
  c_3 = gc;
  o_6 = gz;
  goto __BB4;

  __BB(3):
  t_7 = m_4 ? n_5 : o_6;
  gr = t_7;
  return;

  __BB(4):
  m_4 = a_1 == b_2;
  n_5 = b_2 == c_3;
  goto __BB3;
}

int
main (void)
{
  int a[N], b[N], c[N], z[N], r[N];

  /* Masks with one bit per element, as on x86 with AVX-512, have a different
     layout that the checks below don't decode.  */
  if (sizeof (M) != sizeof (V))
    return 0;

  for (int i = 0; i < N; i++)
    {
      a[i] = i & 1;
      b[i] = (i >> 1) & 1;
      c[i] = (i >> 2) & 1;
      z[i] = (i >> 3) & 1 ? -1 : 0;
    }

  __builtin_memcpy (&ga, a, sizeof (a));
  __builtin_memcpy (&gb, b, sizeof (b));
  __builtin_memcpy (&gc, c, sizeof (c));
  __builtin_memcpy (&gz, z, sizeof (z));

  f ();

  __builtin_memcpy (r, &gr, sizeof (r));
  for (int i = 0; i < N; i++)
    {
      int want = a[i] == b[i] ? (b[i] == c[i] ? -1 : 0) : z[i];
      if (r[i] != want)
	__builtin_abort ();
    }

  return 0;
}
