/* { dg-do run } */
/* { dg-options "-O2 -fno-ipa-icf" } */

/* Like builtin-dynamic-object-size-26.c, but for trailing arrays of the
   same size whose object sizes still differ: one of them can't be a
   flexible array, or one more of its elements doesn't fit in the object
   while one more of the other's does.  -fno-ipa-icf keeps the two sz
   functions apart.  */

typedef __SIZE_TYPE__ size_t;

#define AI static inline __attribute__ ((always_inline))

/* The object has 56 bytes from d on, but e is never a flexible array.  The
   raw member puts a and b in the same position in the union, so they only
   differ in their arrays.  */
struct a1 { long long n; char d[8]; };
struct b1 { long long n; char e[8] __attribute__ ((strict_flex_array (3))); };
union u1 { struct a1 a; struct b1 b; char raw[16]; };
long long blob1[8];

AI char *get_d (union u1 *q) { return q->a.d; }
AI char *get_e (union u1 *q) { return (char *) q->b.e; }

static __attribute__ ((noinline)) size_t
sz1 (char *p)
{
  return __builtin_dynamic_object_size (p, 1);
}

__attribute__ ((noipa)) size_t
call_d (void)
{
  return sz1 (get_d ((union u1 *) blob1));
}

__attribute__ ((noipa)) size_t
call_e (void)
{
  return sz1 (get_e ((union u1 *) blob1));
}

/* The object has 4 bytes past the end of both arrays, room for one more
   char but not for one more long long.  */
struct a2 { char c[16]; };
struct b2 { long long l[2]; };
union u2 { struct a2 x; struct b2 y; long long z; };
char blob2[20] __attribute__ ((aligned (8)));

AI char *get_c (union u2 *q) { return q->x.c; }
AI char *get_l (union u2 *q) { return (char *) q->y.l; }

static __attribute__ ((noinline)) size_t
sz2 (char *p)
{
  return __builtin_dynamic_object_size (p, 1);
}

__attribute__ ((noipa)) size_t
call_c (void)
{
  return sz2 (get_c ((union u2 *) blob2));
}

__attribute__ ((noipa)) size_t
call_l (void)
{
  return sz2 (get_l ((union u2 *) blob2));
}

#define CHECK(v, n) \
  if ((v) != (n) && (v) != (size_t) -1) \
    __builtin_abort ()

int
main (void)
{
  size_t d = call_d (), e = call_e (), c = call_c (), l = call_l ();

  CHECK (d, 56);
  CHECK (e, 8);
  CHECK (c, 20);
  CHECK (l, 16);
  return 0;
}
