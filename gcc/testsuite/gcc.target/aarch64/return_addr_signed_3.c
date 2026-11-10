/* With return address signing, a function that reads the return address
   only on a cold path that doesn't make a call should keep its hot path free
   of the prologue.  The load of the return address from its save slot must
   not be speculated onto the hot path, as if-conversion could do for the
   conditional select in cold_select.  */
/* { dg-do compile } */
/* { dg-options "-O2 -march=armv8.3-a -mbranch-protection=pac-ret -fno-omit-frame-pointer" } */
/* { dg-final { check-function-bodies "**" "" "" { target *-*-* } {\.L[0-9]+} } } */

void *volatile cold_sink;

/*
** cold_store:
**	cmp	x0, x1
**	bgt	.L([0-9]+)
**	add	x0, x0, x1
**	ret
** .L\1:
**	...
**	retaa
*/
long
cold_store (long a, long b)
{
  if (__builtin_expect (a > b, 0))
    cold_sink = __builtin_return_address (0);
  return a + b;
}

/*
** cold_select:
**	cbz	x0, .L([0-9]+)
**	add	x0, x0, x1
**	ret
** .L\1:
**	hint	25 // paciasp
**	...
**	ldr	x0, \[sp, 8\]
**	xpaci	x0
**	...
**	retaa
*/
unsigned long
cold_select (unsigned long a, unsigned long b)
{
  unsigned long r = a + b;
  if (__builtin_expect (a == 0, 0))
    r = (unsigned long) __builtin_return_address (0);
  return r;
}

/* { dg-final { scan-assembler-times {\txpaci\t} 2 } } */
/* { dg-final { scan-assembler-not {xpaclri} } } */
