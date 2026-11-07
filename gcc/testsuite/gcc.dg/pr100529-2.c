/* PR c/100529 */
/* { dg-do compile } */
/* { dg-options "-std=gnu23" } */

/* The size of the result of a conditional is evaluated ahead of its arms.  If
   the type of each arm is declared inside of that arm, nothing can give that
   size and the conditional is an error, rather than an internal compiler
   error.  */

extern int gn (void);

#define MK \
  ({ int m = gn (); struct T { char a[m]; int z; } t; t.z = 1; t; })

int
member (int c)
{
  return (c ? MK : MK).z;  /* { dg-error "type mismatch in conditional expression" } */
}

void
in_auto (int c)
{
  __auto_type x = c ? MK : MK;  /* { dg-error "type mismatch in conditional expression" } */
}

/* An arm can also have a type that's only declared through a comma.  */

int
in_comma (int c)
{
  return (c ? ((void) gn (), MK) : MK).z;  /* { dg-error "type mismatch in conditional expression" } */
}
