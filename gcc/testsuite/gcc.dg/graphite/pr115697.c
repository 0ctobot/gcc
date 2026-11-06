/* PR115697 */
/* { dg-do run } */
/* { dg-options "-O2 -fgraphite-identity -fno-strict-overflow" } */

void
shift (long *c, long n)
{
  long i, j;

  if (n <= 62)
    for (i = n - 2; i >= 0; i--)
      for (j = i; j < n - 1; j++)
	c[j] += c[j + 1];
}

int
main (void)
{
  long c[3] = { 3, 2, 1 };
  volatile long n = 3;

  shift (c, n);
  if (c[0] != 6 || c[1] != 4 || c[2] != 1)
    __builtin_abort ();
  return 0;
}
