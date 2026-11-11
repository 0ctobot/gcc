/* { dg-do run } */

/* Like pta-wide-4.c, but the wide access isn't known to be aligned, either
   because its type says so or because the structures it accesses are
   packed.  The access can then start anywhere in the member that the
   pointer is represented by, and points-to analysis must include the
   member after it, or the copies below are computed to carry only &l2,
   and the store through x is assumed not to modify l1.  */

typedef __INTPTR_TYPE__ v2
  __attribute__ ((vector_size (2 * sizeof (__INTPTR_TYPE__)), may_alias));
typedef v2 v2u __attribute__ ((aligned (1)));

/* The access type has byte alignment.  */

struct s { int *arr[2]; int *c; };

__attribute__((noipa)) int
f1 (int c)
{
  int l1 = 0, l2 = 0;
  struct s s = { { &l2, &l2 }, &l1 }, t = { { &l2, &l2 }, &l1 }, d, e;
  int **p = c ? &s.arr[1] : &t.arr[1];
  int **q = c ? &d.arr[1] : &e.arr[1];
  *(v2u *) q = *(v2u *) p;
  int *x = c ? d.c : e.c;
  l1 = 1;
  *x = 2;
  return l1;
}

/* Both the structures and the access are packed.  */

struct __attribute__ ((packed)) ps { char x; int *arr[2]; int *c; };
struct __attribute__ ((packed, may_alias)) una { v2 v; };

__attribute__((noipa)) int
f2 (int c)
{
  int l1 = 0, l2 = 0;
  struct ps s = { 0, { &l2, &l2 }, &l1 }, t = { 0, { &l2, &l2 }, &l1 }, d, e;
  char *p = c ? (char *) &s.arr[1] : (char *) &t.arr[1];
  char *q = c ? (char *) &d.arr[1] : (char *) &e.arr[1];
  ((struct una *) q)->v = ((struct una *) p)->v;
  int *x = c ? d.c : e.c;
  l1 = 1;
  *x = 2;
  return l1;
}

int
main (void)
{
  if (sizeof (v2) != 2 * sizeof (int *))
    return 0;
  if (f1 (1) != 2 || f1 (0) != 2)
    __builtin_abort ();
  if (f2 (1) != 2 || f2 (0) != 2)
    __builtin_abort ();
  return 0;
}
