/* { dg-options "-fno-lto" } */

extern void f (void);

static void
t (void)
{
  f ();
}

void (*fp) (void) = t;
