/* { dg-do compile } */
/* { dg-options "-O2" } */

/* Each of the two cold paths keeps three values live across a call in a
   loop, and shrink-wrapping saves the registers holding them on that path
   alone.  Both paths should use the same callee-saved registers, since a
   register that the other path already saves still needs a save of its own
   on this path, just like a register that nothing uses yet.  Taking new
   registers instead needs more frame slots.  */

void g (long);

void
f (long *a, long *b, long n, int dir)
{
  if (__builtin_expect (dir == 1, 0))
    {
      long x = a[0], y = a[1], z = a[2];
      for (long i = 0; i < n; i++)
	{
	  g (x);
	  b[i] = x + y * i + z;
	}
    }
  else if (__builtin_expect (dir == 2, 0))
    {
      long u = b[0], v = b[1], w = b[2];
      for (long i = 0; i < n; i++)
	{
	  g (u);
	  a[i] = u * i + v - w;
	}
    }
  else
    a[0] = b[0];
}

/* { dg-final { scan-assembler {\tstp\tx29, x30, \[sp, -80\]!} } } */
/* { dg-final { scan-assembler-not {\tx2[4-8]} } } */
