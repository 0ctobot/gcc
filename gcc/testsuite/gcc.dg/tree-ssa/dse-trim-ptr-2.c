/* { dg-do compile } */
/* { dg-options "-O2 -fchecking=1 -fdump-tree-dse1-details" } */

/* Trimming the head of a clear through a pointer argument rewrites the
   store to use the pointer in a new MEM_REF, so the statement's operands
   have to be updated.  */

struct s { long long a; long long b[8]; };

void
f (struct s *p)
{
  *p = (struct s){};
  p->a = 1;
}

/* { dg-final { scan-tree-dump-times "Trimming statement \\(head = 8, tail = 0\\)" 1 "dse1" } } */
/* { dg-final { scan-tree-dump-times "MEM <char\\\[64\\\]> \\\[\\(struct s \\*\\)p_\[0-9\]+\\(D\\) \\+ 8B\\\] = \{\}" 1 "dse1" } } */
