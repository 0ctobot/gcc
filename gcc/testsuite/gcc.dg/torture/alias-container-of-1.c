/* { dg-do run } */
/* { dg-additional-options "-fno-strict-aliasing" } */

/* A container_of () pointer built from a list head that is a variable
   points below the start of the head, and an access through it at a
   positive offset reads the head.  The stores to the head by name must
   not be assumed to leave the second load of pos->node.next unchanged.  */

#include <stddef.h>

struct list_head { struct list_head *next, *prev; };
struct item { long a; long b; struct list_head node; };

#define container_of(ptr, type, member) \
  ((type *) ((char *) (ptr) - offsetof (type, member)))

struct list_head head = { &head, &head };
struct item it = { 1, 2, { &head, &head } };

__attribute__((noipa)) int
reset_after (struct item *pos)
{
  if (!pos)
    pos = container_of (&head, struct item, node);
  struct list_head *first = pos->node.next;
  head.next = &head;
  head.prev = &head;
  struct list_head *now = pos->node.next;
  return first != now;
}

int
main (void)
{
  head.next = head.prev = &it.node;
  if (reset_after (NULL) != 1)
    __builtin_abort ();
  return 0;
}
