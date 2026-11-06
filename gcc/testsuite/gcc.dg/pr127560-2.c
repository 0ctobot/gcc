/* PR c/127560 */
/* { dg-do compile } */
/* { dg-options "-w" } */

/* Removing a nested C_MAYBE_CONST_EXPR while folding must not turn an
   expression that isn't an integer constant expression into one.  */

static int s = (1 > (1 >> (1 / 0))) ^ 1U ^ 1; /* { dg-error "initializer element is not constant" } */
static int s2 = 1 > 1 >> (-1U >> -__INT_MAX__ ^ __INT_MAX__) ^ 1U ^ 1; /* { dg-error "initializer element is not constant" } */
enum e { A = (1 > (1 >> (1 / 0))) ^ 1U ^ 1 }; /* { dg-error "enumerator value for 'A' is not an integer constant" } */
_Static_assert ((1 > (1 >> (1 / 0))) ^ 1U ^ 1, "x"); /* { dg-error "expression in static assertion is not constant" } */
struct t { int b : (1 > (1 >> (1 / 0))) ^ 1U ^ 1; }; /* { dg-error "bit-field 'b' width not an integer constant" } */
int v[(1 > (1 >> (1 / 0))) ^ 1U ^ 1]; /* { dg-error "variably modified 'v' at file scope" } */

int
f (int p)
{
  static int z = (1 > (1 >> (1 / 0))) ^ 1U ^ 1; /* { dg-error "initializer element is not constant" } */
  switch (p)
    {
    case (1 > (1 >> (1 / 0))) ^ 1U ^ 1: /* { dg-error "case label does not reduce to an integer constant" } */
      return 1;
    }
  return 0;
}
