/* PR c/105430 */
/* { dg-do compile } */
/* { dg-options "-O0 -Woverride-init" } */

/* -Woverride-init reports the part that the whole initializer loses.  */

struct R { int b, c; };
struct S { int a; struct R r; };
struct T { int k; struct S s; };
union U { struct R r; int a; };
struct V { union U u; };

extern struct S f (void);
extern struct S x;
extern union U y;
const union U uc = { .a = 2 };
const struct V vc = { .u = { .a = 2 } };

void
g (void)
{
  struct T a = { .s = x, .s.a = 1 }; /* { dg-warning "initialized field overwritten" } */
  struct T b = { .s = f (), .s.a = 1 }; /* { dg-warning "initialized field overwritten" } */
  struct T c = { .s = x, .s = 9 }; /* { dg-warning "initialized field overwritten" } */
  /* Designating another member of the union replaces the whole value, as it
     does after a braced list.  A part of a member keeps the rest of that
     member, which reports the part it replaces as for a struct.  */
  struct V d = { .u = y, .u.a = 1 }; /* { dg-warning "initialized field overwritten" } */
  struct V e = { .u = { .a = 2 }, .u.a = 1 }; /* { dg-warning "initialized field overwritten" } */
  struct V h = { .u = y, .u.r.b = 1 }; /* { dg-warning "initialized field overwritten" } */
  /* A const object with a constant braced initializer is used as that
     list, so writing a part of another member doesn't warn, as with the
     braced form.  */
  struct V i = { .u = uc, .u.r.b = 1 };
  struct V j = { .u = vc.u, .u.r.b = 1 };
}
