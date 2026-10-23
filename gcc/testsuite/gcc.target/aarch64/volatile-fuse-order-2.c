/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-final { check-function-bodies "**" "" "" { target lp64 } } } */

/* Both reads of *P are in separate full expressions: the first read must
   be the first operand of the compare.  */

/*
** f:
**	ldr	(w[0-9]+), \[x0\]
**	ldr	(w[0-9]+), \[x0\]
**	cmp	\1, \2
**	cset	w0, lt
**	ret
*/

int
f (volatile int *p)
{
  int a = *p;
  int b = *p;
  return a < b;
}
