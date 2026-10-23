/* { dg-do compile } */
/* { dg-options "-O2 -fomit-frame-pointer -fno-pic -masm=att" } */
/* { dg-final { check-function-bodies "**" "" "" { target { ! *-*-darwin* } } } } */

/* An insn does all its reads before its writes, so a volatile read may
   still be combined into a volatile store, and v updated in place.  */

/*
**inc:
**	addl	\$1, v(|\(%rip\))
**	ret
*/

/*
**add_w:
**	movl	w(|\(%rip\)), %[a-z0-9]+
**	addl	%[a-z0-9]+, v(|\(%rip\))
**	ret
*/

extern volatile int v, w;

void
inc (void)
{
  v += 1;
}

void
add_w (void)
{
  v += w;
}
