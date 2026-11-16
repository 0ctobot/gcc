/* { dg-do run { target int32 } } */
/* { dg-options "-O2 -fno-tree-slp-vectorize" } */

/* Store merging can turn the clear of struct F and the two stores through
   union U in callee into the clear and one store of their nonzero bytes,
   leaving the zero bytes of the union stores to the clear.  The load
   through union U in caller conflicts with those stores but not with the
   clear, so it must not take the value stored before the call.  */

union U { int i; short s[2]; };
struct F { long long a[4]; };

static __attribute__((noinline, noclone)) void
callee (struct F *p)
{
  union U *u = (union U *) p;

  *p = (struct F) { 0 };
  u[1].i = 0x01000000;
  u[2].i = 1;
}

__attribute__((noipa)) int
caller (void *m, int flag)
{
  union U *u = m;

  u[2].s[1] = 1;
  if (flag)
    callee (m);
  return u[2].s[1];
}

int
main (void)
{
  union U v = { 1 };
  void *m = __builtin_malloc (sizeof (struct F));
  if (!m)
    return 0;
  if (caller (m, 1) != v.s[1])
    __builtin_abort ();
  __builtin_free (m);
  return 0;
}
