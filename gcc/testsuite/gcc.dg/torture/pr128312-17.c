/* PR tree-optimization/128312 */
/* { dg-do run } */
/* { dg-options "-ftrivial-auto-var-init=zero" } */

/* The first run of an initializer that gets built in a temporary has to read
   the zeroes that the variable starts with however control enters its block:
   at a case label, at a label that a goto from outside of the block jumps to,
   or at the start of the block.  The re-run through the jump back has to read
   the value of the first run instead, and the next entry to the block starts
   over from zero, including one at a case label.  */

struct S { int a[4]; int k; };

__attribute__ ((noipa)) void
clobber (void)
{
  volatile char b[256];
  for (int i = 0; i < 256; i++)
    b[i] = 0x5a;
}

__attribute__ ((noipa)) int
at_case (int n)
{
  int res = 0, runs = 0;
  switch (n)
    {
    case 0:
      res = 5;
      /* FALLTHRU */
    case 1:
    again:;
      struct S v = { .k = 1, .a[0] = runs ? v.k : v.k + 100, .a[1] = 2 };
      res = res * 1000 + v.a[0];
      if (++runs < 2)
	goto again;
    }
  return res;
}

__attribute__ ((noipa)) int
forward_goto (int n)
{
  int res = 0, runs = 0;
  if (n)
    goto again;
  {
    res = 7;
  again:;
    struct S v = { .k = 1, .a[0] = runs ? v.k : v.k + 100, .a[1] = 2 };
    res = res * 1000 + v.a[0];
    if (++runs < 2)
      goto again;
  }
  return res;
}

__attribute__ ((noipa)) int
backward_goto (int n)
{
  int res = 0, runs = 0, once = 0;
  {
    res = 3;
  again:;
    struct S v = { .k = 1, .a[0] = runs ? v.k : v.k + 100, .a[1] = 2 };
    res = res * 1000 + v.a[0];
    if (++runs < 2)
      goto again;
  }
  if (!once++ && n)
    {
      res = 0;
      runs = 0;
      goto again;
    }
  return res;
}

__attribute__ ((noipa)) int
loop_case (int n)
{
  int res = 0;
  for (int i = 0; i < n; i++)
    {
      int runs = 0;
      switch (i)
	{
	case 0:
	case 1:
	again:;
	  struct S v = { .k = 1 + i, .a[0] = runs ? v.k : v.k + 10, .a[1] = 2 };
	  res = res * 100 + v.a[0];
	  if (++runs < 2)
	    goto again;
	}
    }
  return res;
}

int
main (void)
{
  clobber ();
  if (at_case (0) != 5100001)
    __builtin_abort ();
  clobber ();
  if (at_case (1) != 100001)
    __builtin_abort ();
  clobber ();
  if (forward_goto (0) != 7100001)
    __builtin_abort ();
  clobber ();
  if (forward_goto (1) != 100001)
    __builtin_abort ();
  clobber ();
  if (backward_goto (1) != 100001)
    __builtin_abort ();
  clobber ();
  if (loop_case (2) != 10011002)
    __builtin_abort ();
  return 0;
}
