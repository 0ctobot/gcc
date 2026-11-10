/* { dg-options "-fno-lto" } */

extern void f (void);

void
h (void)
{
  f ();
}
