/* { dg-do run } */
/* { dg-additional-options "-std=gnu23" } */

/* A designator for another member of a union after a compound literal of
   that union replaces the member the literal named, as it does after a
   braced list.  An automatic object gets the same bytes as a static one.
   A designator for the same member keeps the rest of that member.  */

union U { int i; char c[4]; long l; };
typedef union U OU;
struct W { long s; union U u; long t; };

__attribute__ ((noipa)) long
g (long x)
{
  return x;
}

__attribute__ ((noipa)) void
check_bytes (long s, const char *c, long t, int c0, int c2, int c3)
{
  if (s != 3 || t != 5
      || c[0] != c0 || c[1] != 9 || c[2] != c2 || c[3] != c3)
    __builtin_abort ();
}

#define check(p, c0, c2, c3) \
  check_bytes ((p)->s, (p)->u.c, (p)->t, c0, c2, c3)

__attribute__ ((noipa)) void
test_other (long n)
{
  struct W a = { 3, .u = (union U) { .l = -1 }, .u.c[1] = 9, .t = 5 };
  struct W b = { 3, .u = (union U) { .l = n }, .u.c[1] = 9, .t = 5 };
  struct W c = { 3, .u = (union U) { .l = g (n) }, .u.c[1] = 9, .t = 5 };
  struct W d = { 3, .u = (union U) { .i = n }, .u.c[1] = 9, .t = 5 };
  static struct W e = { 3, .u = (union U) { .l = -1 }, .u.c[1] = 9, .t = 5 };
  check (&a, 0, 0, 0);
  check (&b, 0, 0, 0);
  check (&c, 0, 0, 0);
  check (&d, 0, 0, 0);
  check (&e, 0, 0, 0);
}

__attribute__ ((noipa)) void
test_same (int n)
{
  struct W a = { 3, .u = (union U) { .c = { 1, n, 3, 4 } }, .u.c[1] = 9,
		 .t = 5 };
  static struct W b = { 3, .u = (union U) { .c = { 1, 2, 3, 4 } },
			.u.c[1] = 9, .t = 5 };
  check (&a, 1, 3, 4);
  check (&b, 1, 3, 4);
}

__attribute__ ((noipa)) void
test_tag (long n)
{
  /* The same tag and members in C23 make a compatible type.  */
  union U { int i; char c[4]; long l; };
  struct V { long s; union U u; long t; };
  struct V a = { 3, .u = (union U) { .l = n }, .u.c[1] = 9, .t = 5 };
  struct V b = { 3, .u = (OU) { .l = n }, .u.c[1] = 9, .t = 5 };
  check (&a, 0, 0, 0);
  check (&b, 0, 0, 0);
}

int
main (void)
{
  test_other (-1);
  test_same (2);
  test_tag (-1);
  return 0;
}
