/* PR tree-optimization/126947 */
/* Merging the two single bit tests must not read the old bit positions after
   setjmp.  */
/* { dg-do run } */

#include <setjmp.h>

jmp_buf env;

__attribute__ ((noipa)) void
escape (int x)
{
  (void) x;
}

__attribute__ ((noipa)) int
f (int a, int n, int m)
{
  int x = (a >> n) & 1, y = (a >> m) & 1;
  escape (0);
  setjmp (env);
  if (x)
    if (y)
      n &= 0;
  escape (0);
  return n;
}

int
main (void)
{
  if (f (6, 1, 2) != 0 || f (2, 1, 2) != 1 || f (4, 1, 2) != 1)
    __builtin_abort ();
  return 0;
}
