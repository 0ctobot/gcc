/* PR tree-optimization/128654 */
/* { dg-do run } */
/* { dg-require-effective-target size32plus } */
/* { dg-options "-O2 -floop-interchange" } */

#define N 64
int a[5 * N][N + 4], b[5 * N][N + 4];

__attribute__((noipa)) void
f (void)
{
  for (int k = 0; k < N; k++)
    for (int i = 0; i < N; i++)
      a[4 * i + k][k + 4] = a[4 * i + k][k] + 1;
}

int
main (void)
{
  f ();
  for (int k = 0; k < N; k++)
    for (int i = 0; i < N; i++)
      {
	b[4 * i + k][k + 4] = b[4 * i + k][k] + 1;
	__asm__ volatile ("" : : : "memory");
      }
  /* a[4][4] is written at (k=0, i=1) before it is read at (k=4, i=0), so
     the loops cannot be interchanged.  */
  for (int r = 0; r < 5 * N; r++)
    for (int c = 0; c < N + 4; c++)
      if (a[r][c] != b[r][c])
	__builtin_abort ();
  return 0;
}
