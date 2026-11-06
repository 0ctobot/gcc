/* PR c/105430 */
/* { dg-do run } */
/* { dg-options "-w" } */

/* An empty member can share its position with the member that follows it, so
   position alone doesn't order the members.  Each pending initializer must stay
   reachable, or else the rest of a whole struct initializer or the side effects
   of the empty member's initializer are lost.  */

extern void abort (void);

struct E {};
struct M { int x; struct E e; int y; };
struct F { struct E e; int x; int y; };
struct L { int x; int y; struct E e; };
struct D { struct E e1; struct E e2; struct E e3; int x; };
struct OM { struct M s; };
struct OF { struct F s; };
struct OL { struct L s; };
struct OD { struct D s; };
struct OOM { int z; struct OM o; };
union A { struct { struct E e; int x; }; int l; };
struct OA { union A u; };

static int cnt;

__attribute__ ((noipa)) struct E
fe (void)
{
  cnt++;
  struct E e;
  return e;
}

__attribute__ ((noipa)) int
fi (int v)
{
  cnt++;
  return v;
}

struct M mv = { .x = 11, .y = 22 };
struct F fv = { .x = 11, .y = 22 };
struct L lv = { .x = 11, .y = 22 };
struct D dv = { .x = 11 };
struct OM omv = { .s = { .x = 11, .y = 22 } };
union A av = { .l = 33 };

#define CHECK(c, n) \
  do \
    { \
      if (!(c) || cnt != (n)) \
	abort (); \
      cnt = 0; \
    } \
  while (0)

__attribute__ ((noipa)) void
whole (void)
{
  {
    struct OM o = { .s = mv, .s.e = fe () };
    CHECK (o.s.x == 11 && o.s.y == 22, 1);
  }
  {
    struct OF o = { .s = fv, .s.e = fe () };
    CHECK (o.s.x == 11 && o.s.y == 22, 1);
  }
  {
    struct OL o = { .s = lv, .s.e = fe () };
    CHECK (o.s.x == 11 && o.s.y == 22, 1);
  }
  {
    struct OD o = { .s = dv, .s.e2 = fe () };
    CHECK (o.s.x == 11, 1);
  }
  {
    struct OOM o = { .z = 1, .o = omv, .o.s.e = fe () };
    CHECK (o.z == 1 && o.o.s.x == 11 && o.o.s.y == 22, 1);
  }
  {
    struct OOM o = { .o.s = mv, .o.s.e = fe () };
    CHECK (o.o.s.x == 11 && o.o.s.y == 22, 1);
  }
  {
    struct OA o = { .u = av, .u.e = fe () };
    CHECK (o.u.x == 33, 1);
  }
  {
    struct OA o = { .u = av, .u.e = ({ fi (0); (struct E) {}; }) };
    CHECK (o.u.x == 33, 1);
  }
}

__attribute__ ((noipa)) void
orders (void)
{
  {
    struct OM o = { .s.y = 22, .s.e = fe () };
    CHECK (o.s.x == 0 && o.s.y == 22, 1);
  }
  {
    struct OM o = { .s.e = fe (), .s.y = 22, .s.x = 11 };
    CHECK (o.s.x == 11 && o.s.y == 22, 1);
  }
  {
    struct OM o = { .s.x = 11, .s.e = fe (), .s.y = 22 };
    CHECK (o.s.x == 11 && o.s.y == 22, 1);
  }
  {
    struct OM o = { .s.y = fi (22), .s.x = fi (11), .s.e = fe () };
    CHECK (o.s.x == 11 && o.s.y == 22, 3);
  }
  {
    struct OF o = { .s.y = 22, .s.x = 11, .s.e = fe () };
    CHECK (o.s.x == 11 && o.s.y == 22, 1);
  }
  {
    struct OF o = { .s.x = 11, .s.e = fe (), .s.y = 22 };
    CHECK (o.s.x == 11 && o.s.y == 22, 1);
  }
  {
    struct OF o = { .s.e = fe (), .s.y = fi (22), .s.x = fi (11) };
    CHECK (o.s.x == 11 && o.s.y == 22, 3);
  }
  {
    struct OL o = { .s.e = fe (), .s.y = 22, .s.x = 11 };
    CHECK (o.s.x == 11 && o.s.y == 22, 1);
  }
  {
    struct OD o = { .s.x = 11, .s.e3 = fe (), .s.e1 = fe (), .s.e2 = fe () };
    CHECK (o.s.x == 11, 3);
  }
  {
    struct OD o = { .s.e3 = fe (), .s.x = fi (11), .s.e1 = fe () };
    CHECK (o.s.x == 11, 3);
  }
  {
    struct OA o = { .u.x = 11, .u.e = fe () };
    CHECK (o.u.x == 11, 1);
  }
  {
    struct OA o = { .u.e = fe (), .u.x = 11 };
    CHECK (o.u.x == 11, 1);
  }
}

/* A second initializer for the same empty member overrides the first, even if
   the first has already been output and the following member is next.  */

__attribute__ ((noipa)) void
override (void)
{
  {
    struct F o = { .e = fe (), .e = fe (), .x = 11, .y = 22 };
    CHECK (o.x == 11 && o.y == 22, 1);
  }
  {
    struct F o = { .x = 11, .e = fe (), .y = 22, .e = fe () };
    CHECK (o.x == 11 && o.y == 22, 1);
  }
  {
    struct M o = { .x = 11, .e = fe (), .e = fe (), .y = 22 };
    CHECK (o.x == 11 && o.y == 22, 1);
  }
  {
    struct M o = { .y = 22, .e = fe (), .x = 11, .e = fe () };
    CHECK (o.x == 11 && o.y == 22, 1);
  }
}

int
main (void)
{
  whole ();
  orders ();
  override ();
  return 0;
}
