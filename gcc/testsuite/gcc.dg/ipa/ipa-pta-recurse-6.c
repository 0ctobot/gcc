/* { dg-do run } */
/* { dg-options "-O2 -fipa-pta" } */

/* The leaf activation of walk stores to a field of its parent's t through
   a pointer moved by a constant offset from the parent's t.tag, which is
   the only field of t in the parameter's points-to set.  Every field of a
   shadowed local needs a shadow, or the store is taken for a store to the
   leaf's own t and the parent's loads of t.a and t.b fold to null.  */

struct node { struct node *child; int val; };
struct three { void *tag; int *a; int *b; };
int g;

static int __attribute__ ((noinline))
walk (struct node *n, void **parent)
{
  struct three t = { 0, 0, 0 };

  if (!n->child)
    {
      if (parent)
	{
	  int **slot;
	  if (n->val & 1)
	    slot = (int **) ((char *) parent
			     + __builtin_offsetof (struct three, a));
	  else
	    slot = (int **) ((char *) parent
			     + __builtin_offsetof (struct three, b));
	  *slot = &g;
	}
      return 0;
    }
  walk (n->child, &t.tag);
  return (t.a == &g) + 2 * (t.b == &g);
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

  if (top (&b) != 1)
    __builtin_abort ();
  return 0;
}
