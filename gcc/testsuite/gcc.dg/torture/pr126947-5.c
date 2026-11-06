/* PR tree-optimization/126947 */
/* Combining the first and last conditions must not move the redefinition of X
   above the second condition, which still reads the old value.  */
/* { dg-do run } */

#include <setjmp.h>

jmp_buf env;

__attribute__ ((noipa)) void
escape (int x)
{
  (void) x;
}

__attribute__ ((noipa)) int
f (int a, int n, int x)
{
  escape (0);
  setjmp (env);
  if (a & n)
    goto out;
  int k = x * 3;
  if (k == 21)
    goto out;
  x = x + 1;
  int m = x << 3;
  if (a & m)
    goto out;
  escape (x);
  return x;
out:
  return 0;
}

int
main (void)
{
  if (f (1, 2, 1) != 2		/* a & n and k == 21 and a & m are all false.  */
      || f (2, 2, 1) != 0	/* a & n is true.  */
      || f (1, 2, 7) != 0	/* k == 21 is true.  */
      || f (16, 2, 1) != 0)	/* a & m is true.  */
    __builtin_abort ();
  return 0;
}
