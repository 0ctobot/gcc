/* { dg-do compile } */
/* { dg-options "-O2 -fomit-frame-pointer" } */
/* Keep labels and directives ('.cfi_startproc', '.cfi_endproc').  */
/* { dg-final { check-function-bodies "**" "*#" "" { target { ! *-*-darwin* } } {^\t?\.} } } */
/* { dg-final { check-function-bodies "*D" "*E" "" { target { *-*-darwin* && lp64 } } {^\t?\.} } } */

/*
**foo:
**.LFB0:
**	.cfi_startproc
**	movl	bar(|\(%rip\)), %eax
**	movl	bar(|\(%rip\)), %eax
**...
**	barrier
**...
**	movl	bar(|\(%rip\)), %[a-z0-9]+
**	addl	%[a-z0-9]+, %eax
**	subl	bar(|\(%rip\)), %eax
**	ret
**	.cfi_endproc
**...
*#

* Darwin indirects the extern.
*Dfoo:
*D	movq	_bar@GOTPCREL\(%rip\), %rdx
*D	movl	\(%rdx\), %eax
*D	movl	\(%rdx\), %eax
*D...
*D	barrier
*D...
*D	movl	\(%rdx\), %[a-z0-9]+
*D	addl	%[a-z0-9]+, %eax
*D	subl	\(%rdx\), %eax
*D	ret
*D...
*E
*/

extern volatile int bar;

int
foo (void)
{
  int h = bar;
  int r = bar;
  asm volatile ("barrier" ::: "memory");
  int p = bar;
  p = p + r;
  return p - bar;
}
