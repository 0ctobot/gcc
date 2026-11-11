/* { dg-do run } */
/* { dg-require-effective-target int128 } */

/* An __int128 access through a pointer to a struct of pointers covers more
   than one member.  Points-to analysis must include every member that the
   access overlaps, not only the one it starts in, or the copies below are
   computed to carry only &l2, and the store through q->b or q->c is assumed
   not to modify l1.  */

typedef unsigned __int128 u128
  __attribute__ ((may_alias, aligned (__alignof__ (int *))));

struct pair { int *a; int *b; };
struct triple { int *a; int *b; int *c; };
union split { u128 w; struct pair p; };

/* Only the load is wide.  */

__attribute__((noipa)) int
f1 (int c)
{
  int l1 = 0, l2 = 0;
  struct pair s = { &l2, &l1 }, t = { &l2, &l1 }, d, e;
  struct pair *p = c ? &s : &t;
  struct pair *q = c ? &d : &e;
  union split u;
  u.w = *(u128 *) p;
  q->a = u.p.a;
  q->b = u.p.b;
  l1 = 1;
  *q->b = 2;
  return l1;
}

/* Only the store is wide.  */

__attribute__((noipa)) int
f2 (int c)
{
  int l1 = 0, l2 = 0;
  struct pair s = { &l2, &l1 }, t = { &l2, &l1 }, d, e;
  struct pair *p = c ? &s : &t;
  struct pair *q = c ? &d : &e;
  union split u;
  u.p.a = p->a;
  u.p.b = p->b;
  *(u128 *) q = u.w;
  l1 = 1;
  *q->b = 2;
  return l1;
}

/* The access starts at a constant offset from the pointer.  */

__attribute__((noipa)) int
f3 (int c)
{
  int l1 = 0, l2 = 0;
  struct triple s = { &l2, &l2, &l1 }, t = { &l2, &l2, &l1 }, d, e;
  struct triple *p = c ? &s : &t;
  struct triple *q = c ? &d : &e;
  *(u128 *) &q->b = *(u128 *) &p->b;
  l1 = 1;
  *q->c = 2;
  return l1;
}

int
main (void)
{
  if (sizeof (struct pair) != sizeof (u128))
    return 0;
  if (f1 (1) != 2 || f1 (0) != 2)
    __builtin_abort ();
  if (f2 (1) != 2 || f2 (0) != 2)
    __builtin_abort ();
  if (f3 (1) != 2 || f3 (0) != 2)
    __builtin_abort ();
  return 0;
}
