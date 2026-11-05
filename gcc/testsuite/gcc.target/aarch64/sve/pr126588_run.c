/* { dg-do run { target aarch64_sve_hw } } */
/* { dg-options "-O3 --param aarch64-autovec-preference=sve-only" } */

/* ivdep rules out dependences between iterations, but it says nothing about the
   three accesses in one iteration.  The two loads form a group that's emitted
   at the first load, which would move the second load before the scatter store.
   There's no runtime alias check for a scatter store, so this can't be
   vectorized that way.  */

int ox[256], oy[256], idx[256];

__attribute__((noipa)) void
foo (int *p, int *q, int n)
{
#pragma GCC ivdep
  for (int i = 0; i < n; i++)
    {
      int x = p[2 * i];
      q[idx[i]] = i + 1;
      int y = p[2 * i + 1];
      ox[i] = x;
      oy[i] = y;
    }
}

int
main (void)
{
  static int a[1024];

  for (int i = 0; i < 64; i++)
    idx[i] = 2 * i + 1;

  foo (a, a, 32);
  for (int i = 0; i < 32; i++)
    if (ox[i] != 0 || oy[i] != i + 1)
      __builtin_abort ();

  return 0;
}
