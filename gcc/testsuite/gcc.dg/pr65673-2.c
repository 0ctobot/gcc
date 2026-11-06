/* PR c/65673 */
/* A compound literal has no decl to find the nonstring attribute on, which
   used to crash the compiler.  An enclosing initializer's field isn't the
   object a nested initializer writes to either, yet it used to be checked in
   its place.  */
/* { dg-do compile } */
/* { dg-options "-Wunterminated-string-initialization" } */

struct n { __attribute__ ((nonstring)) char *x; int y; };
struct t { int y; void *x; int z; };

char (*a)[2] = (char [1][2]){ "ab" }; /* { dg-warning "truncates NUL terminator" } */
struct t b = { .y = 1, .x = (char [1][2]){ "ab" }, .z = 2 }; /* { dg-warning "truncates NUL terminator" } */
struct n c = { .x = (char *) (char [1][2]){ "ab" } }; /* { dg-warning "truncates NUL terminator" } */

void
f (void)
{
  struct n d = { .x = ({ char e[1][2] = { "ab" }; e[0]; }) }; /* { dg-warning "truncates NUL terminator" } */
  struct t g = { .x = ({ __attribute__ ((nonstring)) char h[1][2] = { "ab" }; h[0]; }) };
}
