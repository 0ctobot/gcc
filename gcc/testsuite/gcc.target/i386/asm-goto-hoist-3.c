/* RTL code hoisting used to compute x + 100 before the asm goto that
   sets x, on both the fallthrough and the label path.  */
/* { dg-do run } */
/* { dg-options "-O2 -masm=att" } */

__attribute__((noipa, cold)) int
f (int a)
{
  int x;
  asm goto ("movl %1, %0\n\t"
	    "testl %0, %0\n\t"
	    "jz %l[lab]"
	    : "=r" (x) : "r" (a) : "cc" : lab);
  return (x + 100) ^ 1;
 lab:
  return (x + 100) ^ 2;
}

int
main (void)
{
  if (f (5) != 104 || f (0) != 102)
    __builtin_abort ();
  return 0;
}
