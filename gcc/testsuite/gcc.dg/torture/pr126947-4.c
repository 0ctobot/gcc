/* PR tree-optimization/126947 */
/* Folding the two comparisons into one must not read the old value of X after
   the block has redefined it.  */
/* { dg-do run } */

#include <setjmp.h>

jmp_buf env;

__attribute__ ((noipa)) void
escape (int x)
{
  (void) x;
}

__attribute__ ((noipa)) int
f (int x, int y)
{
  escape (0);
  setjmp (env);
  if (x >= y)
    {
      _Bool t = x != y;
      x = x + 1;
      if (t)
	{
	  escape (x);
	  return x;
	}
    }
  return 0;
}

int
main (void)
{
  if (f (4, 3) != 5 || f (3, 3) != 0 || f (2, 3) != 0)
    __builtin_abort ();
  return 0;
}
