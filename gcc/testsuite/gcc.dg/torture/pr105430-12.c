/* PR c/105430 */
/* { dg-do run } */
/* { dg-options "-std=gnu23 -Wno-override-init" } */

/* The same as pr105430-11.c with a struct literal of a compatible type of
   the same tag, which has fields of its own.  */

union U { int i; char c[4]; long l; };
struct W { union U u; int z; };
struct X { struct W w; int k; };
typedef struct W { union U { int i; char c[4]; long l; } u; int z; } OW;
typedef union U OU;

static struct X sl = { .w = (OW) { .u = (OU) { .l = -1 }, .z = 5 },
		       .w.u.c[1] = 9 };
static struct X sb = { .w = (OW) { .u = { .l = -1 }, .z = 5 },
		       .w.u.c[1] = 9 };

__attribute__ ((noipa)) void
init (struct X *l, struct X *b)
{
  struct X al = { .w = (OW) { .u = (OU) { .l = -1 }, .z = 5 },
		  .w.u.c[1] = 9 };
  struct X ab = { .w = (OW) { .u = { .l = -1 }, .z = 5 },
		  .w.u.c[1] = 9 };
  *l = al;
  *b = ab;
}

static void
check (struct X *x)
{
  if (x->w.u.c[0] != 0 || x->w.u.c[1] != 9 || x->w.u.c[2] != 0
      || x->w.u.c[3] != 0 || x->w.z != 5 || x->k != 0)
    __builtin_abort ();
}

int
main (void)
{
  struct X l, b;
  __builtin_memset (&l, 0x5a, sizeof l);
  __builtin_memset (&b, 0x5a, sizeof b);
  init (&l, &b);
  check (&l);
  check (&b);
  check (&sl);
  check (&sb);
  return 0;
}
