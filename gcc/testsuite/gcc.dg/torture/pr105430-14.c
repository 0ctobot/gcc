/* { dg-do run } */
/* { dg-require-effective-target int32plus } */
/* { dg-options "-Wno-override-init" } */

/* A union set from a member or element of a const object with a constant
   braced initializer and then overridden through another member gets the
   same contents in static and automatic storage.  A static initializer
   folds the member or element to its part of the object's braced list,
   where the designator for the other member replaces the member the list
   initialized.  An automatic initializer that can't use the braced list,
   since the object is volatile, has a nonconstant initializer or is
   indexed by a variable, keeps the bytes it reads from the object.  */

union U { int i; struct { short lo, hi; } s; };
struct HU { int k; union U u; };
struct T { int m; struct HU w; };
struct AU { int k; union U a[2]; };
struct TA { int m; struct AU w; };

const struct HU h0c = { .k = 7, .u = { .i = 0x11223344 } };
const union U ua[2] = { { .i = 0x55667788 }, { .i = 0x11223344 } };
const struct AU a0c = { .k = 7, .a = { { .i = 0x55667788 },
				       { .i = 0x11223344 } } };
const struct T t0c = { .m = 3, .w = { .k = 7, .u = { .i = 0x11223344 } } };
const volatile struct HU vh = { .k = 7, .u = { .i = 0x11223344 } };

static struct HU sh = { .u = h0c.u, .u.s.hi = 0x55 };
static struct HU se = { .u = ua[1], .u.s.hi = 0x55 };
static struct HU sa = { .u = a0c.a[1], .u.s.hi = 0x55 };
static struct T st = { .w = t0c.w, .w.u.s.hi = 0x55 };
static struct TA sta = { .w = a0c, .w.a[1].s.hi = 0x55 };

struct all { struct HU h, e, a, c, v, n, x; struct T t; struct TA ta; };

__attribute__ ((noipa)) void
init (struct all *p, int x, int n)
{
  const struct HU lh = { .k = 7, .u = { .i = 0x11223344 } };
  const struct HU nh = { .k = 7, .u = { .i = x } };
  struct HU h = { .u = h0c.u, .u.s.hi = 0x55 };
  struct HU e = { .u = ua[1], .u.s.hi = 0x55 };
  struct HU a = { .u = a0c.a[1], .u.s.hi = 0x55 };
  struct HU c = { .u = lh.u, .u.s.hi = 0x55 };
  struct HU v = { .u = vh.u, .u.s.hi = 0x55 };
  struct HU nn = { .u = nh.u, .u.s.hi = 0x55 };
  struct HU xx = { .u = ua[n], .u.s.hi = 0x55 };
  struct T t = { .w = t0c.w, .w.u.s.hi = 0x55 };
  struct TA ta = { .w = a0c, .w.a[1].s.hi = 0x55 };
  p->h = h;
  p->e = e;
  p->a = a;
  p->c = c;
  p->v = v;
  p->n = nn;
  p->x = xx;
  p->t = t;
  p->ta = ta;
}

static void
check_folded (const struct HU *h, int k)
{
  if (h->k != k || h->u.s.lo != 0 || h->u.s.hi != 0x55)
    __builtin_abort ();
}

static void
check_bytes (const struct HU *h)
{
  const union U b = { .i = 0x11223344 };
  if (h->k != 0 || h->u.s.lo != b.s.lo || h->u.s.hi != 0x55)
    __builtin_abort ();
}

int
main (void)
{
  struct all p;
  init (&p, 0x11223344, 1);
  check_folded (&sh, 0);
  check_folded (&se, 0);
  check_folded (&sa, 0);
  check_folded (&st.w, 7);
  if (st.m != 0 || sta.m != 0 || sta.w.k != 7 || sta.w.a[0].i != 0x55667788
      || sta.w.a[1].s.lo != 0 || sta.w.a[1].s.hi != 0x55)
    __builtin_abort ();
  check_folded (&p.h, 0);
  check_folded (&p.e, 0);
  check_folded (&p.a, 0);
  check_folded (&p.c, 0);
  check_folded (&p.t.w, 7);
  if (p.t.m != 0 || p.ta.m != 0 || p.ta.w.k != 7
      || p.ta.w.a[0].i != 0x55667788
      || p.ta.w.a[1].s.lo != 0 || p.ta.w.a[1].s.hi != 0x55)
    __builtin_abort ();
  check_bytes (&p.v);
  check_bytes (&p.n);
  check_bytes (&p.x);
  return 0;
}
