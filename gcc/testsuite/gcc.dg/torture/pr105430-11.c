/* { dg-do run } */
/* { dg-options "-Wno-override-init" } */

/* A union inside a compound literal of a struct follows the same rule as a
   compound literal of the union itself: a designator for another member of
   the union replaces the member the literal named.  An automatic object gets
   the same bytes as a static one, whether the union inside the struct
   literal is a literal or a braced list.  The other members of the struct
   literal stay.  */

union U { int i; char c[4]; long l; };
struct W { union U u; int z; };
struct X { struct W w; int k; };

static struct X sl = { .w = (struct W) { .u = (union U) { .l = -1 }, .z = 5 },
		       .w.u.c[1] = 9 };
static struct X sb = { .w = (struct W) { .u = { .l = -1 }, .z = 5 },
		       .w.u.c[1] = 9 };
static struct X sd = { .w = (struct W) { .u = (union U) { .l = -1 }, .z = 5 },
		       .w.u.l = 7 };

__attribute__ ((noipa)) void
init (struct X *l, struct X *b, struct X *d)
{
  struct X al = { .w = (struct W) { .u = (union U) { .l = -1 }, .z = 5 },
		  .w.u.c[1] = 9 };
  struct X ab = { .w = (struct W) { .u = { .l = -1 }, .z = 5 },
		  .w.u.c[1] = 9 };
  struct X ad = { .w = (struct W) { .u = (union U) { .l = -1 }, .z = 5 },
		  .w.u.l = 7 };
  *l = al;
  *b = ab;
  *d = ad;
}

static void
check (struct X *x, struct X *s)
{
  if (__builtin_memcmp (&x->w.u, &s->w.u, sizeof x->w.u.c) != 0
      || x->w.z != 5 || s->w.z != 5 || x->k != 0 || s->k != 0)
    __builtin_abort ();
}

int
main (void)
{
  struct X l, b, d;
  __builtin_memset (&l, 0x5a, sizeof l);
  __builtin_memset (&b, 0x5a, sizeof b);
  __builtin_memset (&d, 0x5a, sizeof d);
  init (&l, &b, &d);
  check (&l, &sl);
  check (&b, &sb);
  check (&d, &sd);
  if (sl.w.u.c[0] != 0 || sl.w.u.c[1] != 9 || sl.w.u.c[2] != 0
      || sb.w.u.c[0] != 0 || sb.w.u.c[1] != 9 || sb.w.u.c[2] != 0
      || d.w.u.l != 7)
    __builtin_abort ();
  return 0;
}
