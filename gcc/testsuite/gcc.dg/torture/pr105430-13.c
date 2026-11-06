/* { dg-do run } */
/* { dg-require-effective-target int32plus } */
/* { dg-options "-Wno-override-init" } */

/* A union set from a const object with a constant braced initializer and
   then overridden through another member gets the same contents in static
   and automatic storage.  A static initializer folds the object to its
   braced list, where the designator for the other member replaces the
   member the list initialized.  The same goes for a union inside such an
   object.  */

union U { int i; struct { short lo, hi; } s; };
struct HU { int k; union U u; };
struct W { union U u; int z; };
struct HW { struct W w; int k; };

const union U u0c = { .i = 0x11223344 };
const struct W w0c = { .u = { .i = 0x11223344 }, .z = 7 };

static struct HU su = { .u = u0c, .u.s.hi = 0x55 };
static struct HW sw = { .w = w0c, .w.u.s.hi = 0x55 };

__attribute__ ((noipa)) void
init (struct HU *u, struct HW *w)
{
  struct HU lu = { .u = u0c, .u.s.hi = 0x55 };
  struct HW lw = { .w = w0c, .w.u.s.hi = 0x55 };
  *u = lu;
  *w = lw;
}

int
main (void)
{
  struct HU u;
  struct HW w;
  init (&u, &w);
  if (su.u.s.lo != 0 || su.u.s.hi != 0x55
      || u.u.s.lo != 0 || u.u.s.hi != 0x55)
    __builtin_abort ();
  if (sw.w.u.s.lo != 0 || sw.w.u.s.hi != 0x55 || sw.w.z != 7
      || w.w.u.s.lo != 0 || w.w.u.s.hi != 0x55 || w.w.z != 7)
    __builtin_abort ();
  return 0;
}
