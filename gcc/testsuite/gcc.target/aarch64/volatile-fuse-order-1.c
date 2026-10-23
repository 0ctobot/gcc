/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-final { check-function-bodies "**" "" "" { target lp64 } } } */

/* The two volatile reads are in separate full expressions, so *x must be
   read before *y.  Combine must not put both reads into one insn, which
   gives them no order.  */

/*
** f:
**	ldr	(w[0-9]+), \[x0\]
**	ldr	(w[0-9]+), \[x1\]
**	cmp	\1, \2
**	cset	w0, lt
**	ret
*/

int
f (volatile int *x, volatile int *y)
{
  int a = *x;
  int b = *y;
  return a < b;
}
