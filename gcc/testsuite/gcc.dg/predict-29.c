/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-profile_estimate" } */

/* A switch case sets the value that __builtin_expect () later calls
   unlikely.  The hint mustn't predict the switch's edge for that case:
   combine_predictions_for_bb () would keep the hint's 10% for it and split
   the other 90% evenly among the remaining 20 edges, which would make the
   unlikely case the most likely one.  The switch keeps even probabilities
   and only the branch on the hinted value is predicted.  */

#define unlikely(x) __builtin_expect (!!(x), 0)

int ext (int);
void slow (void);

#define C(n) case n: a = ext (a + n); break;

int
sw20 (int c, int a)
{
  int err = 0;
  switch (c)
    {
    C (0) C (1) C (2) C (3) C (4) C (5) C (6) C (7) C (8) C (9)
    C (10) C (11) C (12) C (13) C (14) C (15) C (16) C (17) C (18)
    case 19: err = 1; break;
    default: a = ext (a); break;
    }
  if (unlikely (err))
    slow ();
  return a;
}

/* { dg-final { scan-tree-dump-times "__builtin_expect heuristics of edge" 1 "profile_estimate" } } */
/* { dg-final { scan-tree-dump "case 19: <L\[0-9\]+> \\\[4.76%\\\]" "profile_estimate" } } */
/* { dg-final { scan-tree-dump-not "case \[0-9\]+: <L\[0-9\]+> \\\[10.00%\\\]" "profile_estimate" } } */
