/* { dg-do compile } */
/* { dg-options "-O2 -mstrict-align" } */

/* A trimmed clear through a pointer keeps the alignment of the store it
   replaces, so it's still expanded inline under -mstrict-align instead of
   as a call to memset.  */

struct s { long long a; long long b[8]; };

void
f (struct s *p)
{
  *p = (struct s){};
  p->a = 1;
}

void
g (struct s *p, int i)
{
  struct s *q = p + i;
  *q = (struct s){};
  q->b[7] = 1;
}

/* { dg-final { scan-assembler-not "bl\tmemset" } } */
