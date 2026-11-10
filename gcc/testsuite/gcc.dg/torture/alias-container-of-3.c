/* { dg-do run } */
/* { dg-additional-options "-fno-strict-aliasing" } */

/* The container_of () pointer built from the list head is one of ten PHI
   arguments.  Whether the head's address comes first or last, the walk
   that looks for it must not give up with the answer that the pointer
   can't point below the head.  */

#include <stddef.h>

struct list_head { struct list_head *next, *prev; };
struct item { long a; long b; struct list_head node; };

#define container_of(ptr, type, member) \
  ((type *) ((char *) (ptr) - offsetof (type, member)))

struct list_head head = { &head, &head };
struct item it;

__attribute__((noipa)) int
head_last (int k, struct item **v)
{
  struct item *pos;
  switch (k)
    {
    case 1: pos = v[1]; break;
    case 2: pos = v[2]; break;
    case 3: pos = v[3]; break;
    case 4: pos = v[4]; break;
    case 5: pos = v[5]; break;
    case 6: pos = v[6]; break;
    case 7: pos = v[7]; break;
    case 8: pos = v[8]; break;
    case 9: pos = v[9]; break;
    default: pos = container_of (&head, struct item, node); break;
    }
  struct list_head *first = pos->node.next;
  head.next = &head;
  head.prev = &head;
  return first != pos->node.next;
}

__attribute__((noipa)) int
head_first (int k, struct item **v)
{
  struct item *pos;
  switch (k)
    {
    case 0: pos = container_of (&head, struct item, node); break;
    case 1: pos = v[1]; break;
    case 2: pos = v[2]; break;
    case 3: pos = v[3]; break;
    case 4: pos = v[4]; break;
    case 5: pos = v[5]; break;
    case 6: pos = v[6]; break;
    case 7: pos = v[7]; break;
    case 8: pos = v[8]; break;
    default: pos = v[9]; break;
    }
  struct list_head *first = pos->node.next;
  head.next = &head;
  head.prev = &head;
  return first != pos->node.next;
}

int
main (void)
{
  struct item *v[10];
  for (int i = 0; i < 10; i++)
    v[i] = &it;
  head.next = head.prev = &it.node;
  if (head_last (0, v) != 1)
    __builtin_abort ();
  head.next = head.prev = &it.node;
  if (head_first (0, v) != 1)
    __builtin_abort ();
  return 0;
}
