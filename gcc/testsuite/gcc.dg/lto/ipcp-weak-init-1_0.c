/* { dg-lto-do run } */
/* { dg-require-weak "" } */
/* { dg-lto-options {{-O2} {-O2 -flto}} } */

/* T is defined weak here and strong in the other file, so the program uses
   the strong definition.  IPA-CP must not take the target of the indirect
   call in run from the weak initializer, with or without LTO.  */

struct ops { int id; int (*fn) (int); };

static int cb_a (int v) { return v + 1000; }

__attribute__ ((weak)) const struct ops T = { 1, cb_a };

static __attribute__ ((noinline)) int
run (const struct ops *p, int n)
{
  int s = 0;
  for (int i = 0; i < n; i++)
    s += p->fn (i);
  return s;
}

int
use (int n)
{
  return run (&T, n);
}
