/* { dg-do run } */
/* { dg-options "-O2 -fdump-tree-dse1-details" } */

/* DSE trims the head of the clear through Q, a PHI of P and R.  Q only
   turns out to be P once the loop is unrolled, and the pass that then
   removes the PHI must find every use of Q, including the one in the
   trimmed store, or the store is left using a released SSA name.  */

struct s { long long a; long long b[8]; };

__attribute__((noipa)) void
f (struct s *p, struct s *r)
{
  int c = 0;
  for (int k = 0; k < 4; k++)
    c += 2;
  struct s *q = c == 8 ? p : r;
  *q = (struct s){};
  q->a = 1;
}

struct s x, y;

int
main (void)
{
  y.a = 5;
  y.b[3] = 7;
  x.b[3] = 9;
  f (&x, &y);
  if (x.a != 1 || x.b[3] != 0 || y.a != 5 || y.b[3] != 7)
    __builtin_abort ();
  return 0;
}

/* { dg-final { scan-tree-dump-times "Trimming statement \\(head = 8, tail = 0\\)" 1 "dse1" } } */
