/* PR rtl-optimization/128427 */
/* { dg-do run { target lp64 } } */
/* { dg-additional-options "-fno-pie -no-pie" } */

/* The table load is common to both arms of the branches below, so head merging
   in crossjumping tries to move it above the call to maybe_exit.  The index is
   out of bounds on the path where maybe_exit ends the program, so the load
   faults if it runs before the call.  The two copies only get the same base
   register without PIE.  */

#include <stdlib.h>

volatile int sink, sink2, exit_now, take_arm;
static const int tab[4] = { 5, 6, 7, 8 };

__attribute__ ((noipa)) void
maybe_exit (int s)
{
  sink = s;
  if (exit_now)
    exit (0);
}

__attribute__ ((noipa)) void
g (void)
{
  sink++;
}

__attribute__ ((noipa)) int
f (unsigned long y, unsigned long z, int c)
{
  int r, s = tab[z];

  if (c)
    goto out;
  maybe_exit (s);
  if (take_arm)
    {
      r = tab[y];
      g ();
      sink2 += r + y;
      return r - 1;
    }
out:
  r = tab[y];
  g ();
  return r * 3 + s + y;
}

int
main (void)
{
  if (f (2, 1, 1) != 7 * 3 + 6 + 2)
    abort ();
  exit_now = 1;
  take_arm = 1;
  f (0x70000000000000UL, 1, 0);
  abort ();
}
