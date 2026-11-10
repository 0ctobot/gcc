/* A constant index outside the bounds of an array reaches the memory
   around it.  T.b[-1] is T.a[3], the last element of the member before it,
   and M2.m[0][4] and M2.m[0][5] are M2.m[1][0] and M2.m[1][1].  A store
   through such an index must not be moved past or forwarded around an
   access to the element it reaches.  */
/* { dg-do run } */
/* { dg-options "-std=gnu11 -fno-strict-aliasing" } */

struct two { int a[4]; int b[4]; };
struct m2 { int m[2][4]; int tail; };
struct two T;
struct m2 M2;
int out[8];

__attribute__((noipa)) int
g_neg (long i)
{
  T.a[i] = 1;
  T.b[-1] = 2;
  return T.a[i];
}

__attribute__((noipa)) int
p_neg (struct two *t, long i)
{
  t->a[i] = 1;
  t->b[-1] = 2;
  return t->a[i];
}

__attribute__((noipa)) int
g_2d (long j)
{
  M2.m[1][j] = 1;
  M2.m[0][4] = 2;
  return M2.m[1][j];
}

__attribute__((noipa)) int
p_2d (struct m2 *s, long j)
{
  s->m[1][j] = 1;
  s->m[0][4] = 2;
  return s->m[1][j];
}

/* Iteration 3 reads T.a[3] after iteration 0 stored 7 to it.  */
__attribute__((noipa)) void
l_neg (int n)
{
  for (int j = 0; j < n; j++)
    {
      out[j] = T.a[j];
      T.b[-1] = 7;
    }
}

/* Iteration 1 reads M2.m[1][1] after iteration 0 stored 7 to it.  */
__attribute__((noipa)) void
l_2d (int n)
{
  for (int j = 0; j < n; j++)
    {
      out[4 + j] = M2.m[1][j];
      M2.m[0][5] = 7;
    }
}

int
main (void)
{
  if (g_neg (3) != 2 || p_neg (&T, 3) != 2)
    __builtin_abort ();
  if (g_2d (0) != 2 || p_2d (&M2, 0) != 2)
    __builtin_abort ();
  for (int k = 0; k < 4; k++)
    T.a[k] = M2.m[1][k] = 100 + k;
  l_neg (4);
  l_2d (4);
  if (out[3] != 7 || out[5] != 7)
    __builtin_abort ();
  return 0;
}
