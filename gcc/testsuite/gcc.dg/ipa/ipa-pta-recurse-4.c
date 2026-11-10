/* { dg-do run } */
/* { dg-options "-O2 -fipa-pta" } */

/* Like ipa-pta-recurse-1.c, but the parent's context reaches the leaf
   through a static variable instead of a parameter.  c is in the static's
   points-to set, while count is only reachable through c.  */

struct node { struct node *child; int val; };
struct ctx { int *count; };

static struct ctx *cur;

static int __attribute__ ((noinline))
walk (struct node *n)
{
  int count = 0;
  struct ctx c = { &count };
  struct ctx *saved = cur;

  if (!n->child)
    {
      if (saved)
	*saved->count += n->val;
      return 0;
    }
  cur = &c;
  walk (n->child);
  cur = saved;
  return count + n->val;
}

int __attribute__ ((noinline))
top (struct node *n)
{
  return walk (n);
}

int
main (void)
{
  struct node c = { 0, 5 }, b = { &c, 0 };

  if (top (&b) != 5)
    __builtin_abort ();
  return 0;
}
