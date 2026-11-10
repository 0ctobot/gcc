/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-cselim-details" } */

/* The store of 0 to p->a dominates the conditional store, but the volatile
   accesses in between can hand p->a over to another thread.  Making the
   conditional store unconditional would then write p->a behind that
   thread's back.  */

struct S { int lock; int a; };

void
f (struct S *p, int *owner)
{
  p->a = 0;
  *(volatile int *) owner = 1;
  if (*(volatile int *) owner == 2)
    p->a = 1;
}

/* { dg-final { scan-tree-dump-not "Conditional store replacement" "cselim" } } */
