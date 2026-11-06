/* PR target/97863 */
/* { dg-do compile { target lp64 } } */
/* { dg-require-effective-target split_stack } */
/* { dg-options "-O2 -mcmodel=large -fsplit-stack" } */

struct S { char c[4294967296]; };

void g (char *);

void
f (struct S s)
{
  g (s.c);
} /* { dg-message "sorry, unimplemented" } */
