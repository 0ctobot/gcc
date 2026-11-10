/* { dg-do compile } */
/* { dg-options "-O2 -fallow-store-data-races -fdump-tree-cselim-details" } */

/* With -fallow-store-data-races, the volatile accesses between the two
   stores to p->a don't matter.  */

struct S { int lock; int a; };

void
f (struct S *p, int *owner)
{
  p->a = 0;
  *(volatile int *) owner = 1;
  if (*(volatile int *) owner == 2)
    p->a = 1;
}

/* { dg-final { scan-tree-dump "Conditional store replacement" "cselim" } } */
