/* { dg-do compile } */
/* { dg-options "-O2 -fno-pic -masm=att" } */
/* { dg-final { check-function-bodies "**" "" "" { target { ! *-*-darwin* } } } } */

/* hi is read before lo, in separate full expressions.  Combine must not
   put both volatile reads into one insn: the doubleword concat patterns
   accept both halves as memory operands, and the split then reads them
   in its own order.  In g the halves meet in a PLUS rather than an IOR.  */

/*
**f:
**...
**	mov(l|q)	hi(|\(%rip\)), %[a-z0-9]+
**...
**	mov(l|q)	lo(|\(%rip\)), %[a-z0-9]+
**...
*/

/*
**g:
**...
**	mov(l|q)	hi(|\(%rip\)), %[a-z0-9]+
**...
**	(mov|add)(l|q)	lo(|\(%rip\)), %[a-z0-9]+
**...
*/

#ifdef __SIZEOF_INT128__
typedef unsigned __int128 dw;
typedef unsigned long long hw;
#else
typedef unsigned long long dw;
typedef unsigned int hw;
#endif

extern volatile hw hi, lo;

dw
f (dw x)
{
  hw h = hi;
  hw l = lo;
  dw v = ((dw) h << (8 * sizeof (hw))) | l;
  return x + v;
}

dw
g (dw x)
{
  dw hs = (dw) hi << (8 * sizeof (hw));
  hw l = lo;
  dw hsx = hs + x;
  return hsx + l;
}
