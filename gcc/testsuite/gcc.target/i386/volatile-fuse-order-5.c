/* { dg-do compile } */
/* { dg-options "-O2 -fomit-frame-pointer -fno-pic -masm=att" } */
/* { dg-final { check-function-bodies "**" "" "" { target { ! *-*-darwin* } } } } */

/* x is read before the asm that writes volatile z.  An ordinary insn does
   all its reads before its writes, but an asm template may access its
   operands in any order, so combine must not turn the read of x into a
   memory operand of that asm.  The asm in store_two_out has two outputs
   (a PARALLEL); the one in store_one_out has one (a single SET).  */

/*
**store_two_out:
**...
**	movl	x(|\(%rip\)), %e[a-z]+
**...
**	nop # z(|\(%rip\)) %e[a-z]+ %e[a-z]+
**...
*/

/*
**store_one_out:
**...
**	movl	x(|\(%rip\)), %e[a-z]+
**...
**	nop # z(|\(%rip\)) %e[a-z]+
**...
*/

extern volatile int x, z;

int
store_two_out (void)
{
  int a = x;
  int o;
  __asm__ ("nop # %0 %1 %2" : "=m" (z), "=r" (o) : "rm" (a));
  return o;
}

void
store_one_out (void)
{
  int a = x;
  __asm__ ("nop # %0 %1" : "=m" (z) : "rm" (a));
}
