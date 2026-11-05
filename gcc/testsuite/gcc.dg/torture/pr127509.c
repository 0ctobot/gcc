/* PR ipa/127509 */
/* { dg-do run } */
/* { dg-require-alias "" } */

/* noipa on the root of the chain, reached through several plain aliases.  */
__attribute__ ((noipa)) int r0 (int v) { return v; }
extern int r1 (int) __attribute__ ((alias ("r0")));
extern int r2 (int) __attribute__ ((alias ("r1")));
extern int r3 (int) __attribute__ ((alias ("r2")));

/* noipa in the middle of the chain with an inlinable root.  m4 is too far from
   m2 to see it as its immediate target and m0 at the end of the chain lacks
   the attribute.  */
int m0 (int v) { return v; }
extern int m1 (int) __attribute__ ((alias ("m0")));
extern int m2 (int) __attribute__ ((noipa, alias ("m1")));
extern int m3 (int) __attribute__ ((alias ("m2")));
extern int m4 (int) __attribute__ ((alias ("m3")));

/* The same with a root that can't be inlined, which exposes the const flag
   discovered from its body instead.  */
__attribute__ ((noinline)) int n0 (int v) { return v; }
extern int n1 (int) __attribute__ ((alias ("n0")));
extern int n2 (int) __attribute__ ((noipa, alias ("n1")));
extern int n3 (int) __attribute__ ((alias ("n2")));
extern int n4 (int) __attribute__ ((alias ("n3")));

#define CHECK(f)					\
  do							\
    {							\
      int c = f (1) == f (1);				\
      if (__builtin_constant_p (c))			\
	__builtin_abort ();				\
    }							\
  while (0)

int
main (void)
{
  CHECK (r0);
  CHECK (r1);
  CHECK (r2);
  CHECK (r3);
  CHECK (m2);
  CHECK (m3);
  CHECK (m4);
  CHECK (n2);
  CHECK (n3);
  CHECK (n4);
  return 0;
}
