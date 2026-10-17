/* { dg-do run } */

/* PR tree-optimization/127968 */

char b[8] = { 1, 2, 3, 4 };
int c[4];

int
main (void)
{
  for (int k = 0; k < 4; k++)
    {
      int t;
      __builtin_memcpy (&t, b, 4);
      __builtin_memcpy (b + k, &t, 4);
      c[k] = t;
    }

  if (b[6] != 1)
    __builtin_abort ();
  return 0;
}
