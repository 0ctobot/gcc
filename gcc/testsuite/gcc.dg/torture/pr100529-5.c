/* PR c/100529 */
/* A statement expression that declares the type of its own variable-sized
   value.  */
/* { dg-do run } */
/* { dg-require-effective-target int32plus } */
/* { dg-options "-w" } */

extern void abort (void);
extern void *memset (void *, int, __SIZE_TYPE__);

int calls;
__attribute__ ((noipa)) int gn (void) { calls++; return 32; }
__attribute__ ((noipa)) void clob (void)
{
  volatile char v[256];
  for (int i = 0; i < 256; i++)
    v[i] = 0x55;
}

/* The size depends on a local of the statement expression.  */
__attribute__ ((noipa)) int
local (void)
{
  return ({ int m = gn (); struct { char a[m]; } t;
	    memset (t.a, 3, m); t; }).a[31];
}

/* The size depends on a variable set in the statement expression.  */
__attribute__ ((noipa)) int
assigned (void)
{
  int m = 1;
  return ({ m = gn (); struct { char a[m]; } t;
	    memset (t.a, 4, m); t; }).a[31];
}

/* The value has to survive what the rest of the expression does.  */
__attribute__ ((noipa)) int
later (void)
{
  return ({ int m = gn (); struct { char a[m]; } t;
	    memset (t.a, 5, m); t; }).a[31] + (clob (), 1);
}

/* A nested statement expression declares the type.  */
__attribute__ ((noipa)) int
nested (void)
{
  return ({ int k = 6; ({ int m = gn (); struct { char a[m]; } t;
			  memset (t.a, k, m); t; }); }).a[31];
}

/* The value is chosen by a conditional.  */
__attribute__ ((noipa)) int
cond (int c)
{
  return ({ int m = gn (); struct { char a[m]; } t, u;
	    memset (t.a, 7, m); memset (u.a, 8, m); c ? t : u; }).a[31];
}

/* Each evaluation's stack space has to be reclaimed.  */
__attribute__ ((noipa)) int
loop (void)
{
  int r = 0;
  for (int i = 0; i < 4096; i++)
    r += ({ int m = gn () * 1024; struct { char a[m]; } t;
	    memset (t.a, 1, m); t; }).a[31];
  return r;
}

int
main (void)
{
  if (local () != 3 || calls != 1)
    abort ();
  if (assigned () != 4 || calls != 2)
    abort ();
  if (later () != 6 || calls != 3)
    abort ();
  if (nested () != 6 || calls != 4)
    abort ();
  if (cond (1) != 7 || cond (0) != 8 || calls != 6)
    abort ();
  if (loop () != 4096)
    abort ();
  return 0;
}
