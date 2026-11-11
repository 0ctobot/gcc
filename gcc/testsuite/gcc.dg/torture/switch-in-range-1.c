/* { dg-do run } */

/* A switch expanded to a jump table doesn't need the bounds check before
   the table jump when the index is known to be within the case values,
   either because a check of the index in a wider mode came first or
   because of the range of the index.  The table must still send the hole
   at 5 to the default label, and an index outside the case values must
   still get there through the checks that remain, including one that
   matches a case value only in its low 32 bits.  */

/* Calls to distinct functions keep switch conversion from turning the
   switches into lookup tables.  */

#define R(N) __attribute__ ((noipa)) static int r##N (void) { return N; }
R (1) R (2) R (3) R (4) R (5) R (6) R (7) R (8) R (9) R (10) R (11) R (12)
R (13) R (14) R (15) R (16)

/* The case values B to B + 13 except B + 5.  */
#define CASES(B)				\
  case (B) + 0: return r1 ();			\
  case (B) + 1: return r2 ();			\
  case (B) + 2: return r3 ();			\
  case (B) + 3: return r4 ();			\
  case (B) + 4: return r5 ();			\
  case (B) + 6: return r7 ();			\
  case (B) + 7: return r8 ();			\
  case (B) + 8: return r9 ();			\
  case (B) + 9: return r10 ();			\
  case (B) + 10: return r11 ();			\
  case (B) + 11: return r12 ();			\
  case (B) + 12: return r13 ();			\
  case (B) + 13: return r14 ();

#define HOLE(B)					\
  case (B) + 5: return r6 ();

/* Values far enough from the table for switch lowering to split them off
   with a check against the highest value in the table.  */
#define CLUSTER					\
  case 100: case 102: case 104: case 106:	\
  case 108: return r15 ();			\
  case 101: case 103: case 105: case 107:	\
  case 109: return r16 ();

__attribute__ ((noipa)) int
wide (unsigned long long x)
{
  switch (x)
    {
    CASES (0)
    default: return 0;
    }
}

__attribute__ ((noipa)) int
wide_full (unsigned long long x)
{
  switch (x)
    {
    CASES (0)
    HOLE (0)
    default: return 0;
    }
}

__attribute__ ((noipa)) int
wide_neg (long long v)
{
  switch (v)
    {
    CASES (-3)
    default: return 0;
    }
}

__attribute__ ((noipa)) int
split (unsigned long long x)
{
  switch (x)
    {
    CASES (0)
    CLUSTER
    default: return 0;
    }
}

__attribute__ ((noipa)) int
split_full (unsigned long long x)
{
  switch (x)
    {
    CASES (0)
    HOLE (0)
    CLUSTER
    default: return 0;
    }
}

__attribute__ ((noipa)) int
guarded (unsigned long long x)
{
  if (x > 13)
    return -1;
  switch (x)
    {
    CASES (0)
    default: return 0;
    }
}

__attribute__ ((noipa)) int
guarded_full (unsigned long long x)
{
  if (x > 13)
    return -1;
  switch (x)
    {
    CASES (0)
    HOLE (0)
    default: return 0;
    }
}

__attribute__ ((noipa)) int
guarded_int (int i)
{
  if (i < -3 || i > 10)
    return -1;
  switch (i)
    {
    CASES (-3)
    default: return 0;
    }
}

__attribute__ ((noipa)) int
narrow (signed char c)
{
  if (c < -3 || c > 10)
    return -1;
  switch (c)
    {
    CASES (-3)
    default: return 0;
    }
}

__attribute__ ((noipa)) int
narrow_full (signed char c)
{
  if (c < -3 || c > 10)
    return -1;
  switch (c)
    {
    CASES (-3)
    HOLE (-3)
    default: return 0;
    }
}

/* What a switch over CASES (B), plus HOLE (B) if FULL, returns for V.  */

static int
expect (long long v, long long b, int full)
{
  long long d = v - b;

  if (d < 0 || d > 13 || (d == 5 && !full))
    return 0;
  return d + 1;
}

static int
expect_split (unsigned long long x, int full)
{
  if (x >= 100 && x <= 109)
    return x & 1 ? 16 : 15;
  return x > 13 ? 0 : expect (x, 0, full);
}

static int
expect_guarded (long long v, long long b, int full)
{
  if (v < b || v > b + 13)
    return -1;
  return expect (v, b, full);
}

int
main (void)
{
  static const unsigned long long high[] = {
    0x100000000ULL, 0x200000000ULL, 0x8000000000000000ULL,
    0xffffffff00000000ULL
  };

  for (long long k = -40; k < 140; k++)
    {
      unsigned long long x = k;

      if (wide (x) != expect (k, 0, 0)
	  || wide_full (x) != expect (k, 0, 1)
	  || wide_neg (k) != expect (k, -3, 0)
	  || split (x) != expect_split (x, 0)
	  || split_full (x) != expect_split (x, 1)
	  || guarded (x) != (x > 13 ? -1 : expect (k, 0, 0))
	  || guarded_full (x) != (x > 13 ? -1 : expect (k, 0, 1))
	  || guarded_int (k) != expect_guarded (k, -3, 0))
	__builtin_abort ();

      for (unsigned int i = 0; i < sizeof (high) / sizeof (high[0]); i++)
	{
	  unsigned long long h = high[i] + x;

	  if (wide (h) != 0
	      || wide_full (h) != 0
	      || wide_neg ((long long) h) != 0
	      || split (h) != 0
	      || split_full (h) != 0
	      || guarded (h) != -1
	      || guarded_full (h) != -1)
	    __builtin_abort ();
	}
    }

  for (int c = -128; c < 128; c++)
    if (narrow (c) != expect_guarded (c, -3, 0)
	|| narrow_full (c) != expect_guarded (c, -3, 1))
      __builtin_abort ();

  if (guarded_int (-__INT_MAX__ - 1) != -1
      || guarded_int (__INT_MAX__) != -1)
    __builtin_abort ();

  return 0;
}
