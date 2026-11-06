/* PR tree-optimization/128076 */
/* { dg-do run } */

/* The division is only reached once the recursive call returns, which never
   happens, so moving it ahead of the call would trap on targets that fault on
   division by zero.  */

extern void exit (int);

__attribute__ ((noipa)) void
leave (void)
{
  exit (0);
}

__attribute__ ((noipa)) int
f (int n, int d)
{
  if (n == 0)
    {
      leave ();
      return 0;
    }
  int y = f (n - 1, d);
  int z = 100 / d;
  return y + z;
}

int
main (void)
{
  f (1, 0);
  __builtin_abort ();
}
