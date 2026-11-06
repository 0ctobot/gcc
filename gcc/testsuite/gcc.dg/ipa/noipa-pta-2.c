/* All names of a body share one function info in IPA-PTA, so a noipa name has
   to make calls of the body opaque wherever the attribute is along the
   chain.  */
/* { dg-do run } */
/* { dg-require-alias "" } */
/* { dg-options "-O2 -fipa-pta" } */

/* noipa on the alias only.  */
int a0 (int *p) { return *p; }
extern int a1 (int *) __attribute__ ((noipa, alias ("a0")));

/* noipa on the body only.  */
__attribute__ ((noipa)) int b0 (int *p) { return *p; }
extern int b1 (int *) __attribute__ ((alias ("b0")));

/* noipa on a static link in the middle, which resolve_alias takes off the chain
   between c2 and c0.  */
int c0 (int *p) { return *p; }
static int c1 (int *) __attribute__ ((noipa, alias ("c0")));
extern int c2 (int *) __attribute__ ((alias ("c1")));

/* Everything static.  */
static int d0 (int *p) { return *p; }
static int d1 (int *)
  __attribute__ ((noipa, alias ("d0"))) __attribute__ ((used));

#define CHECK(call)					\
  do							\
    {							\
      int l = 4;					\
      int r = call (&l);				\
      int v = l;					\
      if (__builtin_constant_p (v))			\
	__builtin_abort ();				\
      if (r != 4 || v != 4)				\
	__builtin_abort ();				\
    }							\
  while (0)

int
main (void)
{
  CHECK (a1);
  CHECK (b1);
  CHECK (b0);
  CHECK (c2);
  CHECK (d1);
  return 0;
}
