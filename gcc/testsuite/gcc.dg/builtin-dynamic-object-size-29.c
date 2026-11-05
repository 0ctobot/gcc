/* PR tree-optimization/120780 */
/* { dg-do run } */
/* { dg-options "-O2" } */

/* A size from an access attribute only describes the bytes from the pointer
   onward, and the pointer may be as far as PTRDIFF_MAX minus that size into its
   object.  Stepping back from it by any amount up to there must leave the size
   plus the step, even past half of PTRDIFF_MAX.  The pointers are never
   dereferenced.  */

typedef __SIZE_TYPE__ size_t;

#define HALF ((size_t) __PTRDIFF_MAX__ / 2)

static int fails;

#define EXPECT(expr, want) \
  do \
    { \
      if ((expr) != (want)) \
	{ \
	  __builtin_printf ("line %d: %s is %zu, not %zu\n", __LINE__, #expr, \
			    (size_t) (expr), (size_t) (want)); \
	  fails++; \
	} \
    } \
  while (0)

__attribute__ ((noipa, access (read_write, 1, 2))) size_t
back0 (char *p, size_t n, size_t k)
{
  return __builtin_dynamic_object_size (p - k, 0);
}

__attribute__ ((noipa, access (read_write, 1, 2))) size_t
back1 (char *p, size_t n, size_t k)
{
  return __builtin_dynamic_object_size (p - k, 1);
}

__attribute__ ((noipa, access (read_write, 1, 2))) size_t
fwd0 (char *p, size_t n, size_t k)
{
  return __builtin_dynamic_object_size (p + k, 0);
}

__attribute__ ((noipa, access (read_write, 1, 2))) size_t
back_half0 (char *p, size_t n)
{
  return __builtin_dynamic_object_size (p - (HALF + 2), 0);
}

__attribute__ ((noipa, access (read_write, 1, 2))) size_t
back_half1 (char *p, size_t n)
{
  return __builtin_dynamic_object_size (p - (HALF + 2), 1);
}

static char buf[16];

int
main (void)
{
  size_t steps[] = { 0, 16, HALF, HALF + 1, HALF + 2,
		     (size_t) __PTRDIFF_MAX__ - 16 };

  for (unsigned i = 0; i < sizeof (steps) / sizeof (steps[0]); i++)
    {
      EXPECT (back0 (buf, 16, steps[i]), 16 + steps[i]);
      EXPECT (back1 (buf, 16, steps[i]), 16 + steps[i]);
    }
  EXPECT (back_half0 (buf, 16), 16 + HALF + 2);
  EXPECT (back_half1 (buf, 16), 16 + HALF + 2);
  EXPECT (fwd0 (buf, 16, 4), 12);
  EXPECT (fwd0 (buf, 16, 16), 0);
  EXPECT (fwd0 (buf, 16, 17), 0);

  if (fails)
    __builtin_abort ();
  return 0;
}
