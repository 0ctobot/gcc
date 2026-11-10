/* A copy to or from a volatile aggregate must access each byte once, so
   its inline expansion can't finish with an access that overlaps bytes it
   already copied.  Copies of normal memory keep the overlapping tail.  */
/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-final { check-function-bodies "**" "" "" } } */

struct s7 { unsigned char b[7]; };
struct s15 { unsigned char b[15]; };

/*
** cpy7:
**	ldr	(w[0-9]+), \[x1\]
**	ldrh	(w[0-9]+), \[x1, 4\]
**	ldrb	(w[0-9]+), \[x1, 6\]
**	str	\1, \[x0\]
**	strh	\2, \[x0, 4\]
**	strb	\3, \[x0, 6\]
**	ret
*/
void
cpy7 (volatile struct s7 *d, volatile struct s7 *s)
{
  *d = *s;
}

/*
** rd15:
**	ldr	(x[0-9]+), \[x1\]
**	ldr	(w[0-9]+), \[x1, 8\]
**	ldrh	(w[0-9]+), \[x1, 12\]
**	ldrb	(w[0-9]+), \[x1, 14\]
**	str	\1, \[x0\]
**	str	\2, \[x0, 8\]
**	strh	\3, \[x0, 12\]
**	strb	\4, \[x0, 14\]
**	ret
*/
void
rd15 (struct s15 *d, volatile struct s15 *s)
{
  *d = *s;
}

/*
** wr15:
**	...
**	str	x[0-9]+, \[x0\]
**	str	w[0-9]+, \[x0, 8\]
**	strh	w[0-9]+, \[x0, 12\]
**	strb	w[0-9]+, \[x0, 14\]
**	ret
*/
void
wr15 (volatile struct s15 *d, struct s15 *s)
{
  *d = *s;
}

/*
** ncpy7:
**	ldr	(w[0-9]+), \[x1\]
**	ldr	(w[0-9]+), \[x1, 3\]
**	str	\1, \[x0\]
**	str	\2, \[x0, 3\]
**	ret
*/
void
ncpy7 (struct s7 *d, struct s7 *s)
{
  *d = *s;
}
