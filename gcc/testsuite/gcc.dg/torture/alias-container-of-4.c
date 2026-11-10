/* { dg-do run } */
/* { dg-additional-options "-fno-strict-aliasing" } */

/* The pointer is either the head's address converted to the containing
   type, or a container_of () of it that points below the head, so the
   walk reaches the same name with two different offsets.  Whichever of
   the two it sees first, the store to the head by name must not be
   assumed to leave the second load of x->node.next unchanged.  The four
   functions differ only in the order of the two arms.  */

#include <stddef.h>

struct list_head { struct list_head *next, *prev; };
struct item { struct list_head a; struct list_head node; };

#define container_of(ptr, type, member) \
  ((type *) ((char *) (ptr) - offsetof (type, member)))

static struct item d, e;

__attribute__((noipa)) int
g1 (int c, int c2, struct list_head *other, struct list_head *v)
{
  struct list_head *h = c ? &d.a : other;
  struct item *x;
  if (c2)
    x = (struct item *) h;
  else
    x = container_of (h, struct item, node);
  struct list_head *first = x->node.next;
  d.a.next = v;
  return first != x->node.next;
}

__attribute__((noipa)) int
g2 (int c, int c2, struct list_head *other, struct list_head *v)
{
  struct list_head *h = c ? &d.a : other;
  struct item *x;
  if (!c2)
    x = container_of (h, struct item, node);
  else
    x = (struct item *) h;
  struct list_head *first = x->node.next;
  d.a.next = v;
  return first != x->node.next;
}

__attribute__((noipa)) int
g3 (int c, int c2, struct list_head *other, struct list_head *v)
{
  struct list_head *h = c ? &d.a : other;
  struct item *x = c2 ? (struct item *) h
		      : container_of (h, struct item, node);
  struct list_head *first = x->node.next;
  d.a.next = v;
  return first != x->node.next;
}

__attribute__((noipa)) int
g4 (int c, int c2, struct list_head *other, struct list_head *v)
{
  struct list_head *h = c ? &d.a : other;
  struct item *x = !c2 ? container_of (h, struct item, node)
		       : (struct item *) h;
  struct list_head *first = x->node.next;
  d.a.next = v;
  return first != x->node.next;
}

int
main (void)
{
  d.a.next = 0;
  if (g1 (1, 0, &e.node, &d.a) != 1)
    __builtin_abort ();
  d.a.next = 0;
  if (g2 (1, 0, &e.node, &d.a) != 1)
    __builtin_abort ();
  d.a.next = 0;
  if (g3 (1, 0, &e.node, &d.a) != 1)
    __builtin_abort ();
  d.a.next = 0;
  if (g4 (1, 0, &e.node, &d.a) != 1)
    __builtin_abort ();
  return 0;
}
