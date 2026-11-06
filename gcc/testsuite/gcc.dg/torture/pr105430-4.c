/* PR c/105430 */
/* { dg-do run } */
/* { dg-options "" } */

/* Initializing an empty member of a union or struct overrides nothing, so a
   whole initializer listed earlier stays applied, and it used to crash the
   compiler for a union that has a later member.  */

extern void abort (void);

struct E { };
union U { int i; struct E e; int s[3]; };
union L { int s[3]; int i; struct E e; };
union Z { int i; int z[0]; int s[3]; };
union ZL { int s[3]; int i; int z[0]; };
union N { int s[3]; struct { struct E e; }; };
struct P { int a; struct E e; int b; };
struct PL { int a; int b; struct E e; };
struct PZ { int a; int z[0]; int b; };
struct OU { union U u; };
struct OL { union L u; };
struct OZ { union Z u; };
struct OZL { union ZL u; };
struct ON { union N u; };
struct OP { struct P u; };
struct OPL { struct PL u; };
struct OPZ { struct PZ u; };
struct RU { union U u[2]; };
struct RP { struct P u[2]; };

union U a = { .s = { 1, 2, 3 } };
union L l = { .s = { 1, 2, 3 } };
union Z z = { .s = { 1, 2, 3 } };
union ZL zl = { .s = { 1, 2, 3 } };
union N n = { .s = { 1, 2, 3 } };
struct P p = { 1, { }, 2 };
struct PL pl = { 1, 2, { } };
struct PZ pz = { .a = 1, .b = 2 };
int calls;

__attribute__ ((noipa)) union U
mk (void)
{
  calls++;
  return a;
}

__attribute__ ((noipa)) struct E
me (void)
{
  struct E e;
  calls += 10;
  return e;
}

static void
check (const int *s)
{
  if (s[0] != 1 || s[1] != 2 || s[2] != 3)
    abort ();
}

__attribute__ ((noipa)) void
test_unions (void)
{
  struct OU o = { .u = a, .u.e = { } };
  struct OL ol = { .u = l, .u.e = { } };
  struct OZ oz = { .u = z, .u.z = { } };
  struct OZL ozl = { .u = zl, .u.z = { } };
  struct ON on = { .u = n, .u.e = { } };
  check (o.u.s);
  check (ol.u.s);
  check (oz.u.s);
  check (ozl.u.s);
  check (on.u.s);
}

__attribute__ ((noipa)) void
test_structs (void)
{
  struct OP op = { .u = p, .u.e = { } };
  struct OPL opl = { .u = pl, .u.e = { } };
  struct OPZ opz = { .u = pz, .u.z = { } };
  if (op.u.a != 1 || op.u.b != 2
      || opl.u.a != 1 || opl.u.b != 2
      || opz.u.a != 1 || opz.u.b != 2)
    abort ();
}

__attribute__ ((noipa)) void
test_ranges (void)
{
  struct RU r1 = { .u[0 ... 1] = a, .u[1].e = { } };
  struct RU r2 = { .u[0 ... 1] = a, .u[0 ... 1].e = { } };
  struct RP r3 = { .u[0 ... 1] = p, .u[0 ... 1].e = { } };
  check (r1.u[0].s);
  check (r1.u[1].s);
  check (r2.u[0].s);
  check (r2.u[1].s);
  if (r3.u[0].a != 1 || r3.u[0].b != 2 || r3.u[1].a != 1 || r3.u[1].b != 2)
    abort ();
}

__attribute__ ((noipa)) void
test_others (void)
{
  /* The whole value is evaluated once.  */
  struct OU o1 = { .u = mk (), .u.e = { } };
  check (o1.u.s);
  if (calls != 1)
    abort ();

  /* A real member can still replace the part it overlaps afterwards.  */
  struct OU o2 = { .u = a, .u.e = { }, .u.s[1] = 50 };
  if (o2.u.s[0] != 1 || o2.u.s[1] != 50 || o2.u.s[2] != 3)
    abort ();

  /* The side effects of the initializer of an empty member are kept.  */
  calls = 0;
  struct OU o3 = { .u = a, .u.e = me () };
  check (o3.u.s);
  if (calls != 10)
    abort ();

  /* The override doesn't have to come next in the list.  */
  struct { int k; union U u; int m; } o4
    = { .k = 1, .u = a, .m = 2, .u.e = { }, .k = 3 };
  check (o4.u.s);
  if (o4.k != 3 || o4.m != 2)
    abort ();
}

int
main (void)
{
  test_unions ();
  test_structs ();
  test_ranges ();
  test_others ();
  return 0;
}
