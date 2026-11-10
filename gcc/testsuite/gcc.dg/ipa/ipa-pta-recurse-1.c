/* { dg-do run } */
/* { dg-options "-O2 -fipa-pta" } */

/* The leaf activation of walk stores to the count of its parent activation
   through the context the parent passes down.  The parent's count is only
   reachable from the parameter through the parent's c, so IPA PTA has to
   follow c to see that count can belong to another activation.  Otherwise
   the store is deleted as dead at function exit.  */

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
      return 0;
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
