/* { dg-do run } */
/* { dg-options "-O2 -fipa-pta -fdump-ipa-pta2-details" } */

/* The same data flow as ipa-pta-recurse-1.c without recursion: a callee
   updates a local of its caller through a pointer kept in another local of
   the caller.  No activation can see the locals of another one here, so no
   local needs a shadow variable.  */

struct node { struct node *child; int val; };
struct ctx { int *count; };

static void __attribute__ ((noinline))
leaf (struct node *n, struct ctx *parent)
{
  if (parent)
    *parent->count += n->val;
}

static int __attribute__ ((noinline))
walk (struct node *n)
{
  int count = 0;
  struct ctx c = { &count };

  leaf (n->child, &c);
  return count + n->val;
}

static int __attribute__ ((noinline))
sum (struct node *n)
{
  int tmp[4] = { 0 };
  int *p = tmp;
  struct ctx c = { p + 1 };

  leaf (n, &c);
  /* tmp[2] is never read after this store.  */
  tmp[2] = n->val;
  return tmp[1];
}

int __attribute__ ((noinline))
top (struct node *n)
{
  return walk (n) + sum (n);
}

int
main (void)
{
  struct node c = { 0, 5 }, b = { &c, 1 };

  if (top (&b) != 7)
    __builtin_abort ();
  return 0;
}

/* { dg-final { scan-ipa-dump-not "shadow variables" "pta2" } } */
