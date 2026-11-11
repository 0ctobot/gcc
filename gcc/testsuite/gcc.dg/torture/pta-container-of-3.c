/* { dg-do run } */
/* { dg-additional-options "-fno-strict-aliasing" } */

/* container_of () applied to a list head that is a local variable gives a
   pointer to before the start of the head.  The address of a member that
   precedes the list member, here &pos->in, is still before the head, and
   ip->list.next then reads head.next.  Points-to analysis must not treat
   that address as the start of the head, or the load is computed to read
   head.prev and the store through n is missed.  */

#include <stddef.h>

struct list_head { struct list_head *next, *prev; };
struct inner { long x; struct list_head list; };
struct outer { long a; struct inner in; };

#define container_of(ptr, type, member) \
  ((type *) ((char *) (ptr) - offsetof (type, member)))

__attribute__((noipa)) long
f (struct outer *start)
{
  struct list_head head;
  struct outer a, b;
  a.a = 1;
  b.a = 2;
  head.next = &a.in.list;
  head.prev = &b.in.list;
  struct outer *pos = container_of (&head, struct outer, in.list);
  struct inner *ip = start ? &start->in : &pos->in;
  struct outer *n = container_of (ip->list.next, struct outer, in.list);
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
