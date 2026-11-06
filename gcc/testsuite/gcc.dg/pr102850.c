/* PR rtl-optimization/102850 */
/* The volatile read must not be scheduled after the noreturn call to bar.  */
/* { dg-do compile } */
/* { dg-options "-O2 -fsched2-use-superblocks" } */

volatile int x;
int y;
int baz (void);

void
bar (void)
{
  for (;;)
    ++y;
}

void
foo (int a)
{
  int z;
  if (a && baz ())
    {
      z = x;
      bar ();
    }
}
