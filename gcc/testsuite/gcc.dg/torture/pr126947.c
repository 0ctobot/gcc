/* PR tree-optimization/126947 */
/* Merging the two field tests must not read the old value of FLAGS after
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
f (int flags)
{
  int lo = (flags >> 4) & 7, hi = (flags >> 8) & 3;
  escape (0);
  setjmp (env);
  if (lo == 5)
    if (hi == 2)
      flags &= 0;
  escape (0);
  return flags;
}

int
main (void)
{
  if (f (0x250) != 0 || f (0x350) != 0x350 || f (0x240) != 0x240)
    __builtin_abort ();
  return 0;
}
