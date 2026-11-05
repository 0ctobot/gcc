/* If-conversion can leave the flags live on the way into the conversion and
   write them again in the converted sequence, as long as what it writes is a
   copy of the comparison that's already there.  None of these need a branch.  */
/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-final { check-function-bodies "**" "" "" } } */

/*
** cmp_unsigned:
**	cmp	x0, x1
**	cset	w0, hi
**	csinv	w0, w0, wzr, cs
**	ret
*/
int
cmp_unsigned (unsigned long a, unsigned long b)
{
  return a < b ? -1 : a > b;
}

/*
** cmp_signed:
**	cmp	w0, w1
**	cset	w0, lt
**	csinv	w0, w0, wzr, le
**	ret
*/
int
cmp_signed (int a, int b)
{
  return a > b ? -1 : a < b;
}

/*
** carry_chain:
**	cmp	x0, x1
**	cset	x0, hi
**	csel	x0, x0, x2, ne
**	ret
*/
unsigned long
carry_chain (unsigned long a, unsigned long b, unsigned long carry)
{
  return a != b ? a > b : carry;
}
