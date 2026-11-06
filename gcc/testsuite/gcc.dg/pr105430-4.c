/* PR c/105430 */
/* { dg-do compile } */
/* { dg-options "-O0" } */

/* Keeping the rest of a whole initializer doesn't overwrite its side effects,
   so -Woverride-init-side-effects stays quiet unless they get dropped.  */

struct R { int b, c; };
struct S { int a; struct R r; };
struct T { int k; struct S s; };
union U { struct R r; int a; };
struct V { union U u; };

extern struct S f (void);
extern union U fu (void);
extern struct S x;
extern union U y;

void
g (void)
{
  struct T a = { .s = x, .s.a = 1 };
  struct T b = { .s = f (), .s.a = 1 };
  struct V c = { .u = y, .u.a = 1 };
  struct V d = { .u = fu (), .u.r.b = 1 };
  /* Selecting another union member drops the call.  */
  struct V e = { .u = fu (), .u.a = 1 }; /* { dg-warning "initialized field with side-effects overwritten" } */
  struct T h = { .s = f (), .s = x }; /* { dg-warning "initialized field with side-effects overwritten" } */
}
