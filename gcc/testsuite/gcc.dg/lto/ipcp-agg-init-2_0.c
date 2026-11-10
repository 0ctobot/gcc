/* { dg-lto-do run } */
/* { dg-require-alias "" } */
/* { dg-lto-options {{-O3} {-O3 -flto}} } */

/* Layouts that IPA has to step over when it looks up a function pointer in
   a static initializer.  An indirect call that IPA-CP resolves through one
   of these tables must call the same function the program would call
   without optimization.  */

typedef int (*fn_t) (int);

#define CB(n, m, a) \
  static __attribute__ ((noinline)) int n (int v) { return v * m + a; }
CB (c1, 3, 1) CB (c2, 5, 2) CB (c3, 7, 3) CB (c4, 11, 4) CB (c5, 13, 5)
CB (c6, 17, 6) CB (c7, 19, 7) CB (c8, 23, 8) CB (c9, 29, 9) CB (c10, 31, 10)

struct empty { };
union u { long l; fn_t f; char c[16]; };
struct in { int a; fn_t f; };

/* A union holding a function pointer comes before the field that's read.  */
struct su { union u u; fn_t fn; };
static const struct su su1 = { { .f = c1 }, c2 };
static const struct su su2 = { { .l = 0 }, c3 };

static __attribute__ ((noinline)) int
run_su (const struct su *p, int n)
{
  int s = 0;
  for (int i = 0; i < n; i++)
    s += p->fn (i);
  return s;
}

/* Empty structs, bit-fields and a nested array come before the field.  */
struct sb
{
  struct empty e;
  unsigned x : 3, y : 9;
  struct in arr[3];
  struct empty e2;
  fn_t fn;
};
static const struct sb sb1
  = { {}, 1, 2, { { 1, c1 }, { 2, c2 }, { 3, c3 } }, {}, c4 };
static const struct sb sb2
  = { {}, 1, 2, { { 1, c3 }, { 2, c2 }, { 3, c1 } }, {}, c5 };

static __attribute__ ((noinline)) int
run_sb (const struct sb *p, int n)
{
  int s = 0;
  for (int i = 0; i < n; i++)
    s += p->fn (i) + p->arr[2].f (i) + p->arr[1].f (i);
  return s;
}

/* Designators out of order, with omitted (zero) fields in front.  */
struct sd { long pad[4]; struct in a; struct in b; fn_t fn; };
static const struct sd sd1 = { .fn = c6, .b = { .f = c7 }, .a.f = c8 };
static const struct sd sd2 = { .fn = c9, .b.f = c10 };

static __attribute__ ((noinline)) int
run_sd (const struct sd *p, int n)
{
  int s = 0;
  for (int i = 0; i < n; i++)
    s += p->fn (i) + p->b.f (i) + (p->a.f ? p->a.f (i) : -1);
  return s;
}

/* Elements of an array other than the first, and a range designator.  */
static const struct in arr1[6]
  = { [0] = { 0, c1 }, [1 ... 3] = { 1, c2 }, [5] = { 5, c5 } };

static __attribute__ ((noinline)) int
run_arr (const struct in *p, int n)
{
  int s = 0;
  for (int i = 0; i < n; i++)
    s += p->f (i);
  return s;
}

/* An alias of a const table.  */
static const struct in real1 = { 1, c9 };
extern const struct in al1 __attribute__ ((alias ("real1")));

static __attribute__ ((noinline)) int
run_al (const struct in *p, int n)
{
  int s = 0;
  for (int i = 0; i < n; i++)
    s += p->f (i);
  return s;
}

volatile int count = 5;

int
main (void)
{
  int n = count;

  if (run_su (&su1, n) != 60 || run_su (&su2, n) != 85)
    __builtin_abort ();
  if (run_sb (&sb1, n) != 275 || run_sb (&sb2, n) != 250)
    __builtin_abort ();
  if (run_sd (&sd1, n) != 695 || run_sd (&sd2, n) != 690)
    __builtin_abort ();
  if (run_arr (&arr1[0], n) != 35 || run_arr (&arr1[2], n) != 60
      || run_arr (&arr1[3], n) != 60 || run_arr (&arr1[5], n) != 155)
    __builtin_abort ();
  if (run_al (&al1, n) != 335 || run_al (&real1, n) != 335)
    __builtin_abort ();
  return 0;
}
