/* { dg-do run } */
/* { dg-additional-options "-fno-strict-aliasing" } */

/* list_for_each_entry () ends with pos pointing to before the start of the
   local list head, which is computed from the head's address loaded from the
   last entry.  list_for_each_entry_continue () then reads head.next through
   pos->node.next.  Points-to analysis must not treat pos as pointing to the
   start of the head when it applies the negative offset while solving, or that
   load is computed to read head.prev and the store to a.a is missed.  */

#include <stddef.h>

struct list_head { struct list_head *next, *prev; };
struct item { long a; void *b; struct list_head node; };

#define container_of(ptr, type, member) \
  ((type *) ((char *) (ptr) - offsetof (type, member)))
#define list_entry(ptr, type, member) container_of (ptr, type, member)
#define list_first_entry(ptr, type, member) \
  list_entry ((ptr)->next, type, member)
#define list_next_entry(pos, member) \
  list_entry ((pos)->member.next, __typeof__ (*(pos)), member)
#define list_entry_is_head(pos, head, member) (&(pos)->member == (head))
#define list_for_each_entry(pos, head, member) \
  for (pos = list_first_entry (head, __typeof__ (*pos), member); \
       !list_entry_is_head (pos, head, member); \
       pos = list_next_entry (pos, member))
#define list_for_each_entry_continue(pos, head, member) \
  for (pos = list_next_entry (pos, member); \
       !list_entry_is_head (pos, head, member); \
       pos = list_next_entry (pos, member))

__attribute__((noipa)) long
f (void)
{
  struct list_head head;
  struct item a, b;
  struct item *pos;
  long sum = 0;
  a.a = 1;
  b.a = 2;
  head.next = &a.node;
  a.node.prev = &head;
  a.node.next = &b.node;
  b.node.prev = &a.node;
  b.node.next = &head;
  head.prev = &b.node;
  list_for_each_entry (pos, &head, node)
    sum += pos->a;
  list_for_each_entry_continue (pos, &head, node)
    pos->a = 5;
  return a.a + b.a + sum;
}

int
main (void)
{
  if (f () != 13)
    __builtin_abort ();
  return 0;
}
