/* PR c/105430 */
/* { dg-do compile } */
/* { dg-options "-O0 -Woverride-init" } */

/* -Woverride-init reports the part that the whole initializer loses.  */

struct R { int b, c; };
struct S { int a; struct R r; };
struct T { int k; struct S s; };

extern struct S f (void);
extern struct S x;

void
g (void)
{
  struct T a = { .s = x, .s.a = 1 }; /* { dg-warning "initialized field overwritten" } */
  struct T b = { .s = f (), .s.a = 1 }; /* { dg-warning "initialized field overwritten" } */
  struct T c = { .s = x, .s = 9 }; /* { dg-warning "initialized field overwritten" } */
}
