/* Transparent aliases can't chain, so resolve_alias collapses a chain of
   weakrefs when c1 is resolved after b1 and re-points c2 when b2 is resolved
   after c2.  It also re-points the plain aliases c3 and c4 of the static
   weakrefs, which are analyzed after them.  None of these may lose the noipa
   on the b symbols.  */
/* { dg-do run } */
/* { dg-require-alias "" } */
/* { dg-require-weak "" } */
/* { dg-options "-O2" } */

int
a (int v)
{
  return v + 1;
}

static int b1 (int) __attribute__ ((noipa, weakref ("a")));
static int c1 (int) __attribute__ ((weakref ("b1")));

static int c2 (int) __attribute__ ((weakref ("b2")));
static int b2 (int) __attribute__ ((noipa, weakref ("a")));

static int b3 (int) __attribute__ ((noipa, weakref ("a")));
extern int c3 (int) __attribute__ ((alias ("b3")));

extern int c4 (int) __attribute__ ((alias ("b4")));
static int b4 (int) __attribute__ ((noipa, weakref ("a")));

int
main (void)
{
  int x1 = c1 (5);
  int x2 = c2 (5);
  int x3 = c3 (5);
  int x4 = c4 (5);

  if (__builtin_constant_p (x1) || __builtin_constant_p (x2)
      || __builtin_constant_p (x3) || __builtin_constant_p (x4))
    __builtin_abort ();
  return x1 != 6 || x2 != 6 || x3 != 6 || x4 != 6;
}
