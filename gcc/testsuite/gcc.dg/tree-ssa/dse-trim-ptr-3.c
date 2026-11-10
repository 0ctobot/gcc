/* { dg-do compile } */
/* { dg-options "-O2 -fchecking=1 -fdump-tree-dse1-details" } */

/* Trimming the tail of a clear through a pointer computed in the function
   rewrites the store to use that pointer in a new MEM_REF, so the
   statement's operands have to be updated.  */

struct s { long long a; long long b[8]; };

void
g (struct s *p, int i)
{
  struct s *q = p + i;
  *q = (struct s){};
  q->b[7] = 1;
}

/* { dg-final { scan-tree-dump-times "Trimming statement \\(head = 0, tail = 8\\)" 1 "dse1" } } */
/* { dg-final { scan-tree-dump-times "MEM <char\\\[64\\\]> \\\[\\(struct s \\*\\)q_\[0-9\]+\\\] = \{\}" 1 "dse1" } } */
