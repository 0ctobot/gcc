/* { dg-do compile } */
/* { dg-options "-O2 -fno-pic -masm=att" } */
/* { dg-final { check-function-bodies "**" "" "" { target { ! *-*-darwin* } } } } */

/* x, y and z must be read in that order.  With z already a memory operand
   of the last add, combine merged the y load into it, reassociated to
   (t1 + z) + y, and split that so that z was read before y.  */

/*
**f:
**...
**	movl	x(|\(%rip\)), %[a-z0-9]+
**...
**	(movl|addl)	y(|\(%rip\)), %[a-z0-9]+
**...
**	addl	z(|\(%rip\)), %[a-z0-9]+
**...
*/

extern volatile int x, y, z;

int
f (void)
{
  int t1 = x;
  int t2 = y;
  int t3 = z;
  return t1 + t2 + t3;
}
