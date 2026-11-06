/* { dg-do run } */
/* { dg-require-effective-target int32 } */

/* Subtracting an IV from a constant negates its step.  When that step is the
   most negative value of the type, the negation doesn't fit.  Nothing in these
   functions overflows, so the loops must run to completion.  */

/* Constant minus IV.  */
__attribute__((noipa)) int
f1 (int n)
{
  int b = 5, d = 0, k = 0;
  for (;;)
    {
      d = -10 - b;
      if (++k >= n)
	break;
      b += -2147483647 - 1;
    }
  return d;
}

/* ~b folds to -1 - b.  */
__attribute__((noipa)) int
f2 (int n)
{
  int b = 20, d = 0, k = 0;
  for (;;)
    {
      d = ~b;
      if (++k >= n)
	break;
      b += -2147483647 - 1;
    }
  return d;
}

/* The step only becomes the most negative value after the multiplication.  */
__attribute__((noipa)) int
f3 (int n)
{
  int b = 13, d, k = 0;
top:
  d = -414612191 - b * 2;
  if (++k < n)
    {
      b += -1073741824;
      goto top;
    }
  return d;
}

/* The same in a wider type.  */
__attribute__((noipa)) long long
f4 (int n)
{
  long long b = 5, d = 0;
  int k = 0;
  for (;;)
    {
      d = -10 - b;
      if (++k >= n)
	break;
      b += -__LONG_LONG_MAX__ - 1;
    }
  return d;
}

/* The IV starts at zero.  */
__attribute__((noipa)) int
f5 (int n)
{
  int b = 0, d = 0, k = 0;
  for (;;)
    {
      d = -1 - b;
      if (++k >= n)
	break;
      b += -2147483647 - 1;
    }
  return d;
}

/* The minuend evolves in an outer loop.  */
__attribute__((noipa)) int
f6 (int n)
{
  int d = 0;
  for (int x = -10; x < -8; x++)
    {
      int b = 5, k = 0;
      for (;;)
	{
	  d = x - b;
	  if (++k >= n)
	    break;
	  b += -2147483647 - 1;
	}
    }
  return d;
}

int
main (void)
{
  if (f1 (2) != 2147483633)
    __builtin_abort ();
  if (f2 (2) != 2147483627)
    __builtin_abort ();
  if (f3 (2) != 1732871431)
    __builtin_abort ();
  if (f4 (2) != __LONG_LONG_MAX__ - 14)
    __builtin_abort ();
  if (f5 (2) != 2147483647)
    __builtin_abort ();
  if (f6 (2) != 2147483634)
    __builtin_abort ();
  return 0;
}
