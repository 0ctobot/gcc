/* PR tree-optimization/126565 */
/* { dg-do compile } */
/* { dg-options "-O1 -fno-tree-forwprop -fdump-tree-phiopt1" } */

/* The real order reversal done in 8 bits must still be recognized: the
   three-way comparison and the negation should both be gone after phiopt1.  */

int
foo (int x, int y)
{
  signed char c, g;
  unsigned char d, e;
  if (x == y)
    c = 0;
  else if (x < y)
    c = -1;
  else
    c = 1;
  d = c;
  e = -d;
  g = e;
  return g > 0;
}

int
bar (double x, double y)
{
  signed char c, g;
  unsigned char d, e;
  if (x == y)
    c = 0;
  else if (x < y)
    c = -1;
  else if (x > y)
    c = 1;
  else
    c = -128;
  d = c;
  e = -d;
  g = e;
  return g <= 0;
}

/* { dg-final { scan-tree-dump-not "PHI" "phiopt1" } } */
/* { dg-final { scan-tree-dump-not " = -" "phiopt1" } } */
