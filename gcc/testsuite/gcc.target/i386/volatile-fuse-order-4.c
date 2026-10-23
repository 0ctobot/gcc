/* { dg-do compile } */
/* { dg-options "-O2 -fomit-frame-pointer -fno-pic -masm=att" } */
/* { dg-final { check-function-bodies "**" "" "" { target { ! *-*-darwin* } } } } */

/* The two volatile reads are in separate full expressions, so x must be
   read before y.  Combine must not turn both into memory operands of one
   asm, whose template may access them in either order.  The asm in
   two_out has two outputs, so it is a PARALLEL of two SETs rather than a
   single SET.  y may be loaded into a register or stay a memory operand,
   but only after x has been loaded.  */

/*
**two_out:
**...
**	movl	x(|\(%rip\)), %[a-z0-9]+
**...
**	.*[\t ,]y(\(%rip\))?(,.*)?
**...
*/

/*
**one_out:
**...
**	movl	x(|\(%rip\)), %[a-z0-9]+
**...
**	.*[\t ,]y(\(%rip\))?(,.*)?
**...
*/

extern volatile int x, y;

int
two_out (void)
{
  int a = x;
  int b = y;
  int lo, hi;
  __asm__ ("nop # %2 %3" : "=r" (lo), "=r" (hi) : "rm" (a), "rm" (b));
  return lo + hi;
}

int
one_out (void)
{
  int a = x;
  int b = y;
  int lo;
  __asm__ ("nop # %1 %2" : "=r" (lo) : "rm" (a), "rm" (b));
  return lo;
}
