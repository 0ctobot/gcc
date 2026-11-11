/* { dg-do run } */
/* { dg-additional-options "-fipa-pta -Wno-psabi" } */

/* get () reads both members of S with one vector load.  IPA points-to
   analysis must include both members in what that load reads, or the
   returned vector is computed to carry only &l2, and the store through x
   is assumed not to modify l1.  */

typedef __INTPTR_TYPE__ v2
  __attribute__ ((vector_size (2 * sizeof (__INTPTR_TYPE__)), may_alias,
		  aligned (__alignof__ (int *))));

struct pair { int *a; int *b; };

__attribute__((noinline, noclone)) v2
get (struct pair *p)
{
  return *(v2 *) p;
}

int
main (void)
{
  int l1 = 0, l2 = 0;
  struct pair s = { &l2, &l1 };
  if (sizeof (struct pair) != sizeof (v2))
    return 0;
  v2 v = get (&s);
  int *x = (int *) v[1];
  l1 = 1;
  *x = 2;
  if (l1 != 2)
    __builtin_abort ();
  return 0;
}
