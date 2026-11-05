/* PR tree-optimization/120780 */
/* { dg-do run } */
/* { dg-options "-O2" } */

/* A pointer that may come from either a parameter with an access attribute or
   a local array has no single whole size that's right for both: the parameter
   may have any number of bytes before it, while the local array has 4.  A
   whole size taken from the local array would leave the parameter with a size
   of 0 after a step back, so such a step must give at least the real size on
   both paths, which may be an unknown size.  A step forward still gives the
   size minus the step.  The pointers are never dereferenced.  */

typedef __SIZE_TYPE__ size_t;

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

#define AT_LEAST(expr, want) \
  do \
    { \
      if ((expr) < (want)) \
	{ \
	  __builtin_printf ("line %d: %s is %zu, below %zu\n", __LINE__, \
			    #expr, (size_t) (expr), (size_t) (want)); \
	  fails++; \
	} \
    } \
  while (0)

#define BACK(NAME, MODE) \
__attribute__ ((noipa, access (none, 1, 2))) size_t \
NAME (char *p, size_t n, int param) \
{ \
  char local[8]; \
  char *q = param ? p : local + 4; \
  return __builtin_dynamic_object_size (q - 4, MODE); \
}

BACK (back0, 0)
BACK (back1, 1)

__attribute__ ((noipa, access (none, 1, 2))) size_t
var0 (char *p, size_t n, int param, long i)
{
  char local[8];
  char *q = param ? p : local + 4;
  return __builtin_dynamic_object_size (q + i, 0);
}

__attribute__ ((noipa, access (none, 1, 2))) size_t
fwd0 (char *p, size_t n, int param)
{
  char local[8];
  char *q = param ? p : local + 4;
  return __builtin_dynamic_object_size (q + 2, 0);
}

static char buf[12];

int
main (void)
{
  AT_LEAST (back0 (buf + 4, 8, 1), 12);
  AT_LEAST (back1 (buf + 4, 8, 1), 12);
  AT_LEAST (var0 (buf + 4, 8, 1, -4), 12);
  AT_LEAST (back0 (buf + 4, 8, 0), 8);
  AT_LEAST (back1 (buf + 4, 8, 0), 8);
  AT_LEAST (var0 (buf + 4, 8, 0, -4), 8);
  EXPECT (fwd0 (buf + 4, 8, 1), 6);
  EXPECT (fwd0 (buf + 4, 8, 0), 2);

  if (fails)
    __builtin_abort ();
  return 0;
}
