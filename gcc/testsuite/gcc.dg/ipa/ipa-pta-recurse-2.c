/* { dg-do run } */
/* { dg-options "-O2 -fipa-pta" } */

/* Like ipa-pta-recurse-1.c, but the outermost call passes a context it
   never filled in instead of a null pointer, so the result doesn't depend
   on how null pointers are treated.  */

struct node { struct node *child; int val; int has_parent; };
struct ctx { int *count; };

static int __attribute__ ((noinline))
walk (struct node *n, struct ctx *parent)
{
  int count = 0;
  struct ctx c;

  c.count = &count;
  if (!n->child)
    {
      if (n->has_parent)
	*parent->count += n->val;
      return 0;
    }
  walk (n->child, &c);
  return count + n->val;
}

int __attribute__ ((noinline))
top (struct node *n)
{
  struct ctx unused;

  return walk (n, &unused);
}

int
main (void)
{
  struct node c = { 0, 5, 1 }, b = { &c, 0, 0 };

  if (top (&b) != 5)
    __builtin_abort ();
  return 0;
}
