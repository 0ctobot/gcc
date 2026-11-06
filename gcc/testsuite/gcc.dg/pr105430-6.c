/* PR c/105430 */
/* { dg-do compile } */
/* { dg-options "" } */

/* Invalid designators and nonconstant initializers still get their errors
   after an earlier whole initializer.  */

struct R { int b, c; };
struct S { int a; struct R r; };
struct T { int k; struct S s; };
union U { struct R r; int a; };
struct V { union U u; };

extern struct S x;
extern union U y;

struct T t1 = { .s = x, .s.a = 1 }; /* { dg-error "initializer element is not constant" } */
struct V t2 = { .u = y, .u.a = 1 }; /* { dg-error "initializer element is not constant" } */

void
g (void)
{
  struct T a = { .s = x, .s.nosuch = 1 }; /* { dg-error "has no member named" } */
  struct V b = { .u = y, .u.nosuch = 1 }; /* { dg-error "has no member named" } */
}
