/* { dg-do run } */
/* { dg-additional-options "-fno-strict-aliasing" } */

/* container_of () applied to a list head that is a local variable gives a
   pointer to before the start of the head, and pos->node.next then reads
   head.next.  Points-to analysis must not treat pos as pointing to the start
   of the head, or that load is computed to read head.prev and the store
   through n is missed.  */

#include <stddef.h>

struct list_head { struct list_head *next, *prev; };
struct item { long a; long b; struct list_head node; };

#define container_of(ptr, type, member) \
  ((type *) ((char *) (ptr) - offsetof (type, member)))

__attribute__((noipa)) long
f (struct item *start)
{
  struct list_head head;
  struct item a, b;
  a.a = 1;
  b.a = 2;
  head.next = &a.node;
  head.prev = &b.node;
  struct item *pos = start ? start : container_of (&head, struct item, node);
  struct item *n = container_of (pos->node.next, struct item, node);
  n->a = 5;
  return a.a;
}

int
main (void)
{
  if (f (NULL) != 5)
    __builtin_abort ();
  return 0;
}
