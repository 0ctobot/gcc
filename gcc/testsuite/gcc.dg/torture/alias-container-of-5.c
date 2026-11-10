/* { dg-do run } */
/* { dg-require-alias "" } */
/* { dg-additional-options "-fno-strict-aliasing" } */

/* The container_of () pointer is built from one name of a list head and
   the head is stored to through another name of it, which a symbol alias
   provides.  The stores must not be assumed to leave the second load of
   pos->node.next unchanged, whichever names the two sides use.  */

#include <stddef.h>

struct list_head { struct list_head *next, *prev; };
struct item { long a; long b; struct list_head node; };

#define container_of(ptr, type, member) \
  ((type *) ((char *) (ptr) - offsetof (type, member)))

struct list_head head = { &head, &head };
extern struct list_head head_alias __attribute__((alias ("head")));
extern struct list_head head_alias2 __attribute__((alias ("head")));
struct item it;

__attribute__((noipa)) int
from_alias (struct item *pos)
{
  if (!pos)
    pos = container_of (&head_alias, struct item, node);
  struct list_head *first = pos->node.next;
  head.next = &head;
  head.prev = &head;
  return first != pos->node.next;
}

__attribute__((noipa)) int
to_alias (struct item *pos)
{
  if (!pos)
    pos = container_of (&head, struct item, node);
  struct list_head *first = pos->node.next;
  head_alias.next = &head;
  head_alias.prev = &head;
  return first != pos->node.next;
}

__attribute__((noipa)) int
between_aliases (struct item *pos)
{
  if (!pos)
    pos = container_of (&head_alias, struct item, node);
  struct list_head *first = pos->node.next;
  head_alias2.next = &head;
  head_alias2.prev = &head;
  return first != pos->node.next;
}

int
main (void)
{
  head.next = head.prev = &it.node;
  if (from_alias (NULL) != 1)
    __builtin_abort ();
  head.next = head.prev = &it.node;
  if (to_alias (NULL) != 1)
    __builtin_abort ();
  head.next = head.prev = &it.node;
  if (between_aliases (NULL) != 1)
    __builtin_abort ();
  return 0;
}
