/* PR target/54412 */
/* { dg-do run } */
/* { dg-require-effective-target alloca } */
/* { dg-additional-options "-Wno-psabi" } */

/* An over-aligned parameter passed on the stack arrives in a slot aligned only
   as the ABI requires, so the callee needs a local copy once the parameter's
   address is taken.  Each call runs at every 16-byte multiple of the alignment
   boundary, so a slot aligned only to the frame's alignment can't pass by
   luck.  */

typedef long along __attribute__ ((aligned (64)));
typedef _Complex long clong __attribute__ ((aligned (64)));
struct E { } __attribute__ ((aligned (64)));

int fails;

__attribute__ ((noipa)) void
check (const void *p)
{
  if ((__UINTPTR_TYPE__) p & 63)
    fails++;
}

__attribute__ ((noipa)) long
scalars (long a, long b, long c, long d, long e, long f, long g, long h,
	 along x, along y)
{
  check (&x);
  check (&y);
  return a + b + c + d + e + f + g + h + x + y;
}

__attribute__ ((noipa)) long
complex_scalar (long a, long b, long c, long d, long e, long f, long g, long h,
		clong x)
{
  check (&x);
  return a + b + c + d + e + f + g + h + __real__ x + __imag__ x;
}

/* A zero-size parameter has no storage, but its address must still be aligned
   both when it would come first and when the registers have run out.  */

__attribute__ ((noipa)) long
empty_first (struct E e, long v)
{
  check (&e);
  return v;
}

__attribute__ ((noipa)) long
empty_last (long a, long b, long c, long d, long e, long f, long g, long h,
	    long i, struct E z)
{
  check (&z);
  return a + b + c + d + e + f + g + h + i;
}

int
main (void)
{
  struct E z;

  for (int i = 1; i <= 8; i++)
    {
      char *volatile p = __builtin_alloca (16 * i);
      p[0] = 0;
      if (scalars (1, 2, 3, 4, 5, 6, 7, 8, 9 * i, 10) != 46 + 9 * i)
	__builtin_abort ();
      if (complex_scalar (1, 2, 3, 4, 5, 6, 7, 8, i + 20i) != 36 + i + 20)
	__builtin_abort ();
      if (empty_first (z, i) != i)
	__builtin_abort ();
      if (empty_last (1, 2, 3, 4, 5, 6, 7, 8, i, z) != 36 + i)
	__builtin_abort ();
    }
  if (fails)
    __builtin_abort ();
  return 0;
}
