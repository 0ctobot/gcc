/* PR tree-optimization/85803 */
/* A store to a member of the object that the pointer points to can't fault
   when the store before it through that pointer didn't, so the first store
   of s1 is dead.  The middle stores of s2 and s3 aren't known to be inside
   that object, so the first stores there must stay.  */
/* { dg-do compile } */
/* { dg-options "-O2 -fno-strict-aliasing -fno-store-merging -fdump-tree-dse1-details" } */

struct s { long a, b; };

void
s1 (struct s *p)
{
  p->a = 1;
  p->b = 7;
  p->a = 2;
}

void
s2 (long *p)
{
  p[0] = 3;
  p[512] = 7;
  p[0] = 4;
}

void
s3 (long *p, long i)
{
  p[0] = 5;
  p[i] = 7;
  p[0] = 6;
}

/* { dg-final { scan-tree-dump-times "Deleted dead store" 1 "dse1" } } */
/* { dg-final { scan-tree-dump "Deleted dead store: \[^\n\]*= 1;" "dse1" } } */
