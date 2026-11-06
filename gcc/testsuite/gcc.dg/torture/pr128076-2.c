/* PR tree-optimization/128076 */
/* { dg-do run } */
/* { dg-require-effective-target trapping } */
/* { dg-additional-options "-ftrapv" } */

/* The additions don't overflow in the order written, but they do when summed
   starting from the outermost call, which must not trap.  */

__attribute__ ((noipa)) int
f (int n, int x, int y)
{
  if (n == 0)
    return 0;
  if (n == 1)
    return f (n - 1, x, y) + x;
  return f (n - 1, x, y) + y;
}

int
main (void)
{
  if (f (3, __INT_MAX__, -__INT_MAX__) != -__INT_MAX__)
    __builtin_abort ();
  return 0;
}
