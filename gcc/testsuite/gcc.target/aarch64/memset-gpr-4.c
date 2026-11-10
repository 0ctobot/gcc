/* Without SIMD, a memset of fewer than 8 bytes replicates the fill byte
   only across as many bytes as its widest store, in a W register, rather
   than building the full 64-bit pattern.  */
/* { dg-do compile } */
/* { dg-options "-O2 -mgeneral-regs-only" } */
/* { dg-final { check-function-bodies "**" "" "" } } */

/*
** v2:
**	ubfiz	(w[0-9]+), w1, 8, 8
**	add	(w[0-9]+), \1, w1, uxtb
**	strh	\2, \[x0\]
**	ret
*/
void
v2 (char *p, int c)
{
  __builtin_memset (p, c, 2);
}

/*
** v5:
**	and	(w[0-9]+), w1, 255
**	mov	(w[0-9]+), 16843009
**	mul	(w[0-9]+), \1, \2
**	str	\3, \[x0\]
**	strb	\3, \[x0, 4\]
**	ret
*/
void
v5 (char *p, int c)
{
  __builtin_memset (p, c, 5);
}

/*
** c3:
**	mov	(w[0-9]+), 23130
**	strh	\1, \[x0\]
**	strb	\1, \[x0, 2\]
**	ret
*/
void
c3 (char *p)
{
  __builtin_memset (p, 0x5a, 3);
}

/*
** c6:
**	mov	(w[0-9]+), 23130
**	movk	\1, 0x5a5a, lsl 16
**	str	\1, \[x0\]
**	strh	\1, \[x0, 4\]
**	ret
*/
void
c6 (char *p)
{
  __builtin_memset (p, 0x5a, 6);
}
