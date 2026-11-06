/* PR target/97863 */
/* { dg-do compile { target lp64 } } */
/* { dg-require-effective-target split_stack } */
/* { dg-options "-O2 -mcmodel=large -fsplit-stack" } */

void g (char *);

void
f (void)
{
  char c[4294967296];
  g (c);
} /* { dg-message "sorry, unimplemented" } */
