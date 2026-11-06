/* PR tree-optimization/126947 */
/* Testing both conditions at the inner one must not read the old value of X
   after the block has redefined it.  */
/* { dg-do run } */

#include <setjmp.h>

jmp_buf env;

__attribute__ ((noipa)) void
escape (int x)
{
  (void) x;
}

__attribute__ ((noipa)) int
f (int x, unsigned u)
{
  escape (0);
  setjmp (env);
  if (x > 3)
    {
      x = (int) u;
      if (x > 5)
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
  if (f (4, 6) != 6 || f (3, 6) != 0 || f (4, 5) != 0)
    __builtin_abort ();
  return 0;
}
