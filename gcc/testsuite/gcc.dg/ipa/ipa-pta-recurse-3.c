/* { dg-do run } */
/* { dg-options "-O2 -fipa-pta" } */

/* Like ipa-pta-recurse-1.c, but the leaf reads its own count after the
   store, so the store isn't dead.  If the store through parent->count is
   taken for a store to walk's own local, walk is found to be pure and the
   parent keeps count == 0 across the recursive call.  */

struct node { struct node *child; int val; };
struct ctx { int *count; };

static int __attribute__ ((noinline))
walk (struct node *n, struct ctx *parent)
{
  int count = 0;
  struct ctx c = { &count };

  if (!n->child)
    {
      if (parent)
	*parent->count += n->val;
      return count;
    }
  walk (n->child, &c);
  return count + n->val;
}

int __attribute__ ((noinline))
top (struct node *n)
{
  return walk (n, 0);
}

int
main (void)
{
  struct node c = { 0, 5 }, b = { &c, 0 };

  if (top (&b) != 5)
    __builtin_abort ();
  return 0;
}
