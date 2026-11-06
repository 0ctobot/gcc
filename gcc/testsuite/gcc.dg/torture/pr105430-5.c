/* PR c/105430 */
/* { dg-do run } */
/* { dg-options "" } */

/* A const-qualified access doesn't mean the object can't change, so a whole
   initializer read through one must be read just once when a later designator
   overrides part of it, or when a range designator repeats it.  The same goes
   for arithmetic on such an access as the value of a range designator.  The
   other initializer may run before or after the read, but it can't split the
   value into parts of both.  Overriding an element of such a range warns under
   -Woverride-init-side-effects, as it does when the value is read through a
   pointer to non-const.  */

extern void abort (void);

struct S { int a, m, z; };
struct T { struct S s; int k; };
union U { int i[3]; char c[12]; };
struct UT { union U u; };
struct A { int r[3]; };
struct AO { struct A t; };
struct W { struct T t; };
struct R { struct S s[2]; };

struct S obj = { 1, 2, 3 };
union U uo = { .i = { 1, 2, 3 } };
struct A ao = { { 1, 2, 3 } };
struct W wo = { { { 1, 2, 3 }, 4 } };
struct S tab[2] = { { 1, 2, 3 }, { 4, 5, 6 } };

__attribute__ ((noipa)) int
mod (void)
{
  obj.a = 10;
  obj.z = 30;
  uo.i[0] = 10;
  uo.i[2] = 30;
  ao.r[0] = 10;
  ao.r[2] = 30;
  wo.t.s.a = 10;
  wo.t.s.z = 30;
  tab[0].a = 10;
  tab[0].z = 30;
  return 5;
}

static void
check (int a, int m, int z)
{
  if (m != 5 || !((a == 1 && z == 3) || (a == 10 && z == 30)))
    abort ();
}

__attribute__ ((noipa)) struct T
test_ptr (const struct S *cp)
{
  struct T t = { .s = *cp, .s.m = mod () };
  return t;
}

__attribute__ ((noipa)) struct T
test_elt (const struct S *cp, int i)
{
  struct T t = { .s = cp[i], .s.m = mod () };
  return t;
}

__attribute__ ((noipa)) struct UT
test_union (const union U *cp)
{
  struct UT t = { .u = *cp, .u.i[1] = mod () };
  return t;
}

__attribute__ ((noipa)) struct AO
test_array (const struct A *cp)
{
  struct AO t = { .t = *cp, .t.r[1] = mod () };
  return t;
}

__attribute__ ((noipa)) struct W
test_nested (const struct W *cp)
{
  struct W w = { .t = cp->t, .t.s.m = mod () };
  return w;
}

__attribute__ ((noipa)) struct R
test_range (const struct S *cp)
{
  struct R r = { .s[0 ... 1] = *cp, .s[0].m = mod () };
  return r;
}

__attribute__ ((noipa)) void
test_range_sum (const struct S *cp, int *v)
{
  int a[3] = { [0 ... 2] = cp->a + cp->z, [1] = mod () }; /* { dg-warning "overwritten" } */
  __builtin_memcpy (v, a, sizeof a);
}

__attribute__ ((noipa)) void
test_range_neg (const struct S *cp, int *v)
{
  int a[3] = { [0 ... 2] = -cp->a, [1] = mod () }; /* { dg-warning "overwritten" } */
  __builtin_memcpy (v, a, sizeof a);
}

__attribute__ ((noipa)) void
test_range_cond (const struct S *cp, int *v)
{
  int a[3] = { [0 ... 2] = cp->a == 1 ? cp->z : 0, [1] = mod () }; /* { dg-warning "overwritten" } */
  __builtin_memcpy (v, a, sizeof a);
}

static void
check_range (const int *v, int before, int after)
{
  if (v[1] != 5 || v[2] != v[0] || (v[0] != before && v[0] != after))
    abort ();
}

static void
reset (void)
{
  obj = (struct S) { 1, 2, 3 };
  tab[0] = obj;
  uo = (union U) { .i = { 1, 2, 3 } };
  ao = (struct A) { { 1, 2, 3 } };
  wo.t.s = obj;
}

int
main (void)
{
  struct T t = test_ptr (&obj);
  check (t.s.a, t.s.m, t.s.z);

  reset ();
  t = test_elt (tab, 0);
  check (t.s.a, t.s.m, t.s.z);

  reset ();
  struct UT ut = test_union (&uo);
  check (ut.u.i[0], ut.u.i[1], ut.u.i[2]);

  reset ();
  struct AO ao2 = test_array (&ao);
  check (ao2.t.r[0], ao2.t.r[1], ao2.t.r[2]);

  reset ();
  struct W w = test_nested (&wo);
  check (w.t.s.a, w.t.s.m, w.t.s.z);
  if (w.t.k != 4)
    abort ();

  reset ();
  struct R r = test_range (&obj);
  check (r.s[0].a, r.s[0].m, r.s[0].z);
  if (r.s[1].a != r.s[0].a || r.s[1].m != 2 || r.s[1].z != r.s[0].z)
    abort ();

  int v[3];
  reset ();
  test_range_sum (&obj, v);
  check_range (v, 4, 40);

  reset ();
  test_range_neg (&obj, v);
  check_range (v, -1, -10);

  reset ();
  test_range_cond (&obj, v);
  check_range (v, 3, 0);

  return 0;
}
