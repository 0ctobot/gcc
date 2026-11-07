/* PR c/105430 */
/* { dg-do run } */
/* { dg-options "-std=gnu23" } */

/* A compound literal of a union type with the same tag as a union declared in
   another scope can initialize a member of that type.  A designator into the
   same union member must then keep the rest of that member, as it does for a
   literal of the same type.  */

struct P { int a[2]; int b; };
union U { int i; struct P p; };
struct S { int n; union U u; };
typedef union U OU;
typedef struct S OS;

extern void abort (void);

int
main (void)
{
  union U { int i; struct P p; };
  struct S { int n; union U u; };

  /* The literal's type is the outer union.  */
  static struct W { union U u; } a = {
    .u = (OU) { .p = { { 1, 2 }, 3 } },
    .u.p.a[0] = 9
  };
  if (a.u.p.a[0] != 9 || a.u.p.a[1] != 2 || a.u.p.b != 3)
    abort ();

  static struct W b = {
    .u = (OU) { .p = { { 1, 2 }, 3 } },
    .u.p.b = 9
  };
  if (b.u.p.a[0] != 1 || b.u.p.a[1] != 2 || b.u.p.b != 9)
    abort ();

  /* The union is inside of a literal of a struct type.  */
  static struct X { struct S s; } c = {
    .s = (OS) { .n = 7, .u = { .p = { { 1, 2 }, 3 } } },
    .s.u.p.a[1] = 9
  };
  if (c.s.n != 7 || c.s.u.p.a[0] != 1 || c.s.u.p.a[1] != 9 || c.s.u.p.b != 3)
    abort ();

  /* Designating another member replaces the literal's member.  */
  static struct W d = {
    .u = (OU) { .p = { { 1, 2 }, 3 } },
    .u.i = 9
  };
  if (d.u.i != 9 || d.u.p.a[1] != 0 || d.u.p.b != 0)
    abort ();

  /* The same forms in automatic storage.  */
  struct W e = {
    .u = (OU) { .p = { { 1, 2 }, 3 } },
    .u.p.a[0] = 9
  };
  if (e.u.p.a[0] != 9 || e.u.p.a[1] != 2 || e.u.p.b != 3)
    abort ();

  return 0;
}
