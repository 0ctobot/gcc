/* PR tree-optimization/128654 */
/* { dg-do run } */
/* { dg-options "-O2 -ftree-loop-distribution" } */

#define N 32
int a[2 * N][N + 1], c[N][N], e[N][N];

__attribute__((noipa)) void
f (void)
{
  for (int i = 0; i < N; i++)
    for (int k = 0; k < N; k++)
      {
	a[i + k][k + 1] = 1;
	c[i][k] = a[i + k][k];
	e[i][k] = 0;
      }
}

int
main (void)
{
  f ();
  /* a[1][1] is read at (i=0, k=1) before it is written at (i=1, k=0).  */
  if (c[0][1] != 0)
    __builtin_abort ();
  return 0;
}
