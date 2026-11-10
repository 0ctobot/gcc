/* Without SIMD, small memsets of a zero, constant or variable byte are
   expanded inline with stores from general registers.  Fills larger than
   256 bytes and fills with -mstrict-align still call memset.  */
/* { dg-do compile } */
/* { dg-options "-O2 -mgeneral-regs-only" } */
/* { dg-final { check-function-bodies "**" "" "" } } */

#define Z(n) void z##n (char *p) { __builtin_memset (p, 0, n); }
#define C(n) void c##n (char *p) { __builtin_memset (p, 0x5a, n); }
#define V(n) void v##n (char *p, int c) { __builtin_memset (p, c, n); }

/*
** z23:
**	stp	xzr, xzr, \[x0\]
**	str	xzr, \[x0, 15\]
**	ret
*/
Z (23)
Z (40)
Z (256)

/*
** c23:
**	mov	(x[0-9]+), 23130
**	movk	\1, 0x5a5a, lsl 16
**	orr	\1, \1, \1, lsl 32
**	stp	\1, \1, \[x0\]
**	str	\1, \[x0, 15\]
**	ret
*/
C (23)
C (40)
C (256)

/*
** v40:
**	and	(x[0-9]+), x1, 255
**	mov	(x[0-9]+), 72340172838076673
**	mul	(x[0-9]+), \1, \2
**	stp	\3, \3, \[x0\]
**	stp	\3, \3, \[x0, 16\]
**	str	\3, \[x0, 32\]
**	ret
*/
V (23)
V (40)
V (256)

void
z257 (char *p)
{
  __builtin_memset (p, 0, 257);
}

__attribute__ ((target ("strict-align"))) void
s40 (char *p)
{
  __builtin_memset (p, 0x5a, 40);
}

/* { dg-final { scan-assembler-times {\tstp\txzr, xzr, \[x0} 19 } } */
/* { dg-final { scan-assembler-times {\tbl?\tmemset} 2 } } */
