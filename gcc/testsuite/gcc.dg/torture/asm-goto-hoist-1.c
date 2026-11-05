/* RTL code hoisting must not insert a computation that reads an asm goto
   output in front of the asm goto: the asm goto is the block's final jump
   and it is what sets the output.  The "cold" attribute makes the hoist pass
   run at -O2 and -O3 too, not only at -Os.  */
/* { dg-do run { target asm_goto_with_outputs } } */

int g;

__attribute__((noipa)) void
use (int v)
{
  g = v;
}

__attribute__((noipa, cold)) int
f1 (int a)
{
  int x;
  asm goto ("" : "=r" (x) : "0" (a) : : lab);
  return (x + 100) ^ 1;
 lab:
  return (x + 100) ^ 2;
}

__attribute__((noipa, cold)) int
f2 (int a)
{
  int x;
  asm goto ("" : "=r" (x) : "0" (a) : : lab);
  use (x + 100);
  return 1;
 lab:
  use (x + 100);
  return 2;
}

int
main (void)
{
  if (f1 (5) != 104)
    __builtin_abort ();
  if (f2 (7) != 1 || g != 107)
    __builtin_abort ();
  return 0;
}
