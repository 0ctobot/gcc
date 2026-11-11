/* { dg-do run } */

/* A pointer to the second element of an array member is represented by the
   whole member, so points-to analysis doesn't know where in the member a
   wide access through that pointer starts.  It must still include the
   member after it that the access can reach, or the copies below are
   computed to carry only &l2, and the store through x is assumed not to
   modify l1.  */

typedef __INTPTR_TYPE__ v2
  __attribute__ ((vector_size (2 * sizeof (__INTPTR_TYPE__)), may_alias,
		  aligned (__alignof__ (int *))));

struct s { int *arr[2]; int *c; };
union split { v2 w; int *p[2]; };

/* Only the load is wide.  */

__attribute__((noipa)) int
f1 (int c)
{
  int l1 = 0, l2 = 0;
  struct s s = { { &l2, &l2 }, &l1 }, t = { { &l2, &l2 }, &l1 };
  int **p = c ? &s.arr[1] : &t.arr[1];
  union split u;
  u.w = *(v2 *) p;
  int *x = u.p[1];
  l1 = 1;
  *x = 2;
  return l1;
}

/* Only the store is wide.  */

__attribute__((noipa)) int
f2 (int c)
{
  int l1 = 0, l2 = 0;
  struct s d, e;
  int **q = c ? &d.arr[1] : &e.arr[1];
  union split u;
  u.p[0] = &l2;
  u.p[1] = &l1;
  *(v2 *) q = u.w;
  int *x = c ? d.c : e.c;
  l1 = 1;
  *x = 2;
  return l1;
}

/* Both are wide.  */

__attribute__((noipa)) int
f3 (int c)
{
  int l1 = 0, l2 = 0;
  struct s s = { { &l2, &l2 }, &l1 }, t = { { &l2, &l2 }, &l1 }, d, e;
  int **p = c ? &s.arr[1] : &t.arr[1];
  int **q = c ? &d.arr[1] : &e.arr[1];
  *(v2 *) q = *(v2 *) p;
  int *x = c ? d.c : e.c;
  l1 = 1;
  *x = 2;
  return l1;
}

/* The access starts at the second member of a structure inside a union,
   and the union is kept as one field.  */

struct u { union { struct { int *x, *y; } in; __INTPTR_TYPE__ l[2]; } u;
	   int *c; };

__attribute__((noipa)) int
f4 (int c)
{
  int l1 = 0, l2 = 0;
  struct u s, t, d, e;
  s.u.in.x = s.u.in.y = t.u.in.x = t.u.in.y = &l2;
  s.c = t.c = &l1;
  int **p = c ? &s.u.in.y : &t.u.in.y;
  int **q = c ? &d.u.in.y : &e.u.in.y;
  *(v2 *) q = *(v2 *) p;
  int *x = c ? d.c : e.c;
  l1 = 1;
  *x = 2;
  return l1;
}

/* A memcpy of two pointers that gets folded into one wide load and store.  */

__attribute__((noipa)) int
f5 (int c)
{
  int l1 = 0, l2 = 0;
  struct s s = { { &l2, &l2 }, &l1 }, t = { { &l2, &l2 }, &l1 }, d, e;
  int **p = c ? &s.arr[1] : &t.arr[1];
  int **q = c ? &d.arr[1] : &e.arr[1];
  __builtin_memcpy (q, p, 2 * sizeof (int *));
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
  if (f3 (1) != 2 || f3 (0) != 2)
    __builtin_abort ();
  if (f4 (1) != 2 || f4 (0) != 2)
    __builtin_abort ();
  if (f5 (1) != 2 || f5 (0) != 2)
    __builtin_abort ();
  return 0;
}
