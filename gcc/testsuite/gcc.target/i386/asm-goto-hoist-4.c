/* RTL code hoisting used to move the load of *p in front of an asm goto
   without outputs that stores to *p and, as the manual asks, has a
   "memory" clobber.  */
/* { dg-do run } */
/* { dg-options "-O2 -masm=att" } */

static int v;

__attribute__((noipa, cold)) int
f (int *p, int a)
{
  asm goto ("movl %1, (%0)\n\t"
	    "testl %1, %1\n\t"
	    "jz %l[lab]"
	    : : "r" (p), "r" (a) : "cc", "memory" : lab);
  return (*p + 100) ^ 1;
 lab:
  return (*p + 100) ^ 2;
}

int
main (void)
{
  v = 1;
  if (f (&v, 5) != 104)
    __builtin_abort ();
  v = 1;
  if (f (&v, 0) != 102)
    __builtin_abort ();
  return 0;
}
