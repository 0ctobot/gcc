/* { dg-do run } */
/* { dg-require-effective-target int32plus } */

/* A switch on a 64-bit or wider index that is expanded to a jump table must
   send an index that matches a case value only in its low 32 bits to the
   default label, whether the range of the index is unknown, known but wider
   than 32 bits, or known to fit after subtracting the lowest case value.
   The functions ending in _ok have an index range at the limit of what
   lets the wide bounds check go away, and those ending in _bad have a range
   one value past it, which a value below takes into the table once it's
   truncated to 32 bits.  */

/* Calls to distinct functions keep switch conversion from turning the
   switches into lookup tables.  */

#define R(N) __attribute__ ((noipa)) static int r##N (void) { return N; }
R (1) R (2) R (3) R (4) R (5) R (6) R (7) R (8) R (9) R (10) R (11) R (12)

#define CASES(B)				\
  case (B) + 0: return r1 ();			\
  case (B) + 1: return r2 ();			\
  case (B) + 2: return r3 ();			\
  case (B) + 3: return r4 ();			\
  case (B) + 4: return r5 ();			\
  case (B) + 5: return r6 ();			\
  case (B) + 6: return r7 ();			\
  case (B) + 7: return r8 ();			\
  case (B) + 8: return r9 ();			\
  case (B) + 9: return r10 ();			\
  case (B) + 10: return r11 ();			\
  case (B) + 11: return r12 ();

#define TWO32 0x100000000LL

__attribute__ ((noipa)) int
full (unsigned long long x)
{
  switch (x)
    {
    CASES (5)
    default: return 0;
    }
}

__attribute__ ((noipa)) int
partial (unsigned long long x)
{
  switch (x & 0x1ffffffffULL)
    {
    CASES (5)
    default: return 0;
    }
}

__attribute__ ((noipa)) int
masked (unsigned long long x)
{
  switch (x & 63)
    {
    CASES (5)
    default: return 0;
    }
}

__attribute__ ((noipa)) int
masked_signed (unsigned long long x)
{
  long long v = (long long) (x & 63) - 8;

  switch (v)
    {
    CASES (0)
    default: return 0;
    }
}

__attribute__ ((noipa)) int
full_signed (long long x)
{
  switch (x)
    {
    CASES (-6)
    default: return 0;
    }
}

/* The index minus the lowest case value is at most 2^32 - 1.  */
__attribute__ ((noipa)) int
hi_ok (unsigned long long x)
{
  if (x > TWO32 + 4)
    return -1;
  switch (x)
    {
    CASES (5)
    default: return 0;
    }
}

/* The index minus the lowest case value can be 2^32.  */
__attribute__ ((noipa)) int
hi_bad (unsigned long long x)
{
  if (x > TWO32 + 5)
    return -1;
  switch (x)
    {
    CASES (5)
    default: return 0;
    }
}

/* The index can be 12 - 2^32, which truncates to 12.  */
__attribute__ ((noipa)) int
lo_ok (long long v)
{
  if (v < 12 - TWO32 || v > 1000)
    return -1;
  switch (v)
    {
    CASES (0)
    default: return 0;
    }
}

/* The index can be 11 - 2^32, which truncates to 11.  */
__attribute__ ((noipa)) int
lo_bad (long long v)
{
  if (v < 11 - TWO32 || v > 1000)
    return -1;
  switch (v)
    {
    CASES (0)
    default: return 0;
    }
}

/* A negative lowest case value and an index with the range of int.  */
__attribute__ ((noipa)) int
sext (int i)
{
  long long v = i;

  switch (v)
    {
    CASES (-6)
    default: return 0;
    }
}

/* Bit-fields wider than 32 bits.  */
struct bf { unsigned long long u : 40; long long s : 40; };

__attribute__ ((noipa)) int
bf_full (const struct bf *p)
{
  switch (p->u)
    {
    CASES (5)
    default: return 0;
    }
}

__attribute__ ((noipa)) int
bf_ok (const struct bf *p)
{
  if (p->s < -50 || p->s > 50)
    return -1;
  switch (p->s)
    {
    CASES (-6)
    default: return 0;
    }
}

__attribute__ ((noipa)) int
bf_hi_bad (const struct bf *p)
{
  if (p->u > TWO32 + 5)
    return -1;
  switch (p->u)
    {
    CASES (5)
    default: return 0;
    }
}

__attribute__ ((noipa)) int
bf_lo_bad (const struct bf *p)
{
  if (p->s < 11 - TWO32 || p->s > 5)
    return -1;
  switch (p->s)
    {
    CASES (0)
    default: return 0;
    }
}

#ifdef __SIZEOF_INT128__
__attribute__ ((noipa)) int
i128_full (__int128 v)
{
  switch (v)
    {
    CASES (5)
    default: return 0;
    }
}

__attribute__ ((noipa)) int
i128_ok (__int128 v)
{
  if (v < -5 || v > TWO32 + 4)
    return -1;
  switch (v)
    {
    CASES (5)
    default: return 0;
    }
}

__attribute__ ((noipa)) int
i128_bad (__int128 v)
{
  if (v < -5 || v > TWO32 + 5)
    return -1;
  switch (v)
    {
    CASES (5)
    default: return 0;
    }
}
#endif

static int
expect (long long x, long long lo)
{
  return x >= lo && x < lo + 12 ? x - lo + 1 : 0;
}

/* Values next to the case range in their low 32 bits and at the guards of
   the functions above.  */
static void
check_edges (long long k)
{
  struct bf b;

  if (hi_ok (k) != expect (k, 5)
      || hi_ok (TWO32 + k) != (k <= 4 ? 0 : -1)
      || hi_bad (k) != expect (k, 5)
      || hi_bad (TWO32 + k) != (k <= 5 ? 0 : -1)
      || lo_ok (k - 8) != expect (k - 8, 0)
      || lo_ok (12 - TWO32 + k) != 0
      || lo_ok (11 - TWO32 - k) != -1
      || lo_bad (k - 8) != expect (k - 8, 0)
      || lo_bad (11 - TWO32 + k) != 0
      || lo_bad (10 - TWO32 - k) != -1
      || sext ((int) k - 32) != expect (k - 32, -6)
      || sext (-0x7fffffff - 1 + (int) k) != 0
      || sext (0x7fffffff - (int) k) != 0)
    __builtin_abort ();

  b.u = k;
  b.s = k - 32;
  if (bf_full (&b) != expect (k, 5)
      || bf_ok (&b) != (k < 32 - 50 || k > 32 + 50 ? -1 : expect (k - 32, -6))
      || bf_hi_bad (&b) != expect (k, 5)
      || bf_lo_bad (&b) != (k > 37 ? -1 : expect (k - 32, 0)))
    __builtin_abort ();

  b.u = TWO32 + k;
  b.s = 11 - TWO32 + k;
  if (bf_full (&b) != 0
      || bf_hi_bad (&b) != (k <= 5 ? 0 : -1)
      || bf_lo_bad (&b) != 0)
    __builtin_abort ();

#ifdef __SIZEOF_INT128__
  __int128 one = 1;

  if (i128_full (k) != expect (k, 5)
      || i128_full ((one << 64) + k) != 0
      || i128_full ((one << 32) + k) != 0
      || i128_full (-(one << 64) + k) != 0
      || i128_ok (k - 5) != expect (k - 5, 5)
      || i128_ok (TWO32 + k) != (k <= 4 ? 0 : -1)
      || i128_bad (k - 5) != expect (k - 5, 5)
      || i128_bad (TWO32 + k) != (k <= 5 ? 0 : -1))
    __builtin_abort ();
#endif
}

int
main (void)
{
  static const unsigned long long high[] = {
    0x100000000ULL, 0x200000000ULL, 0x8000000000000000ULL,
    0xffffffff00000000ULL
  };

  for (long long k = 0; k < 64; k++)
    {
      if (full (k) != expect (k, 5)
	  || partial (k) != expect (k, 5)
	  || masked (k) != expect (k, 5)
	  || masked_signed (k) != expect (k - 8, 0)
	  || full_signed (k - 32) != expect (k - 32, -6))
	__builtin_abort ();

      for (unsigned int i = 0; i < sizeof (high) / sizeof (high[0]); i++)
	{
	  unsigned long long h = high[i];

	  if (full (h + k) != 0
	      || masked (h + k) != expect (k, 5)
	      || masked_signed (h + k) != expect (k - 8, 0)
	      || full_signed ((long long) (h + k - 32)) != 0
	      || full_signed ((long long) (k - 32 - h)) != 0)
	    __builtin_abort ();
	  if (partial (h + k) != (h & 0x100000000ULL ? 0 : expect (k, 5)))
	    __builtin_abort ();
	}

      check_edges (k);
    }
  return 0;
}
