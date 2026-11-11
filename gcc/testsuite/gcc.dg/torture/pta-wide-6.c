/* { dg-do run } */
/* { dg-additional-options "-fipa-pta -Wno-psabi" } */

/* get () reads the second element of an array member and the member after
   it with one vector load, through a pointer that IPA points-to analysis
   represents by the whole array member.  It must still include the member
   after it in what that load reads, or the returned vector is computed to
   carry only &l2, and the store through x is assumed not to modify l1.  */

typedef __INTPTR_TYPE__ v2
  __attribute__ ((vector_size (2 * sizeof (__INTPTR_TYPE__)), may_alias,
		  aligned (__alignof__ (int *))));

struct s { int *arr[2]; int *c; };

__attribute__((noinline, noclone)) v2
get (int **p)
{
  return *(v2 *) p;
}

int
main (void)
{
  int l1 = 0, l2 = 0;
  struct s s = { { &l2, &l2 }, &l1 };
  if (sizeof (v2) != 2 * sizeof (int *))
    return 0;
  v2 v = get (&s.arr[1]);
  int *x = (int *) v[1];
  l1 = 1;
  *x = 2;
  if (l1 != 2)
    __builtin_abort ();
  return 0;
}
