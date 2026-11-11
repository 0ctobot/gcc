/* { dg-do run } */

/* A vector access or a memcpy through a pointer to a struct of pointers
   covers more than one member.  Points-to analysis must include every
   member that the access overlaps, not only the one it starts in, or the
   copies below are computed to carry only &l2, and the store through q->b
   is assumed not to modify l1.  */

typedef __INTPTR_TYPE__ v2
  __attribute__ ((vector_size (2 * sizeof (__INTPTR_TYPE__)), may_alias,
		  aligned (__alignof__ (int *))));

struct pair { int *a; int *b; };
struct __attribute__ ((may_alias)) wrap { v2 v; };

/* The vector is accessed directly through a MEM_REF.  */

__attribute__((noipa)) int
f1 (int c)
{
  int l1 = 0, l2 = 0;
  struct pair s = { &l2, &l1 }, t = { &l2, &l1 }, d, e;
  struct pair *p = c ? &s : &t;
  struct pair *q = c ? &d : &e;
  *(v2 *) q = *(v2 *) p;
  l1 = 1;
  *q->b = 2;
  return l1;
}

/* The vector is a member accessed through a COMPONENT_REF.  */

__attribute__((noipa)) int
f2 (int c)
{
  int l1 = 0, l2 = 0;
  struct pair s = { &l2, &l1 }, t = { &l2, &l1 }, d, e;
  struct pair *p = c ? &s : &t;
  struct pair *q = c ? &d : &e;
  ((struct wrap *) q)->v = ((struct wrap *) p)->v;
  l1 = 1;
  *q->b = 2;
  return l1;
}

/* A memcpy of the whole struct can be folded into a single wide load and
   store.  */

__attribute__((noipa)) int
f3 (int c)
{
  int l1 = 0, l2 = 0;
  struct pair s = { &l2, &l1 }, t = { &l2, &l1 }, d, e;
  struct pair *p = c ? &s : &t;
  struct pair *q = c ? &d : &e;
  __builtin_memcpy (q, p, sizeof (struct pair));
  l1 = 1;
  *q->b = 2;
  return l1;
}

int
main (void)
{
  if (sizeof (struct pair) != sizeof (v2))
    return 0;
  if (f1 (1) != 2 || f1 (0) != 2)
    __builtin_abort ();
  if (f2 (1) != 2 || f2 (0) != 2)
    __builtin_abort ();
  if (f3 (1) != 2 || f3 (0) != 2)
    __builtin_abort ();
  return 0;
}
