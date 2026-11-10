/* { dg-do compile } */
/* { dg-options "-O2 -fno-strict-aliasing -fdump-tree-fre1" } */

/* An ordinary pointer isn't known to point below the variable head, so an
   access at offset 16 through it still can't reach the 16-byte head and
   the second load of p->node.next is redundant.  */

struct list_head { struct list_head *next, *prev; };
struct item { long a; long b; struct list_head node; };

struct list_head head;

int
f (struct item *p)
{
  struct list_head *a = p->node.next;
  head.next = 0;
  return a == p->node.next;
}

/* { dg-final { scan-tree-dump "return 1;" "fre1" } } */
