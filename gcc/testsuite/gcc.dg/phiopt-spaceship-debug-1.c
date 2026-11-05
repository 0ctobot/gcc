/* { dg-do compile } */
/* { dg-options "-O1 -g -fno-tree-forwprop -fdump-tree-phiopt1" } */

/* The second comparison has its operands swapped relative to the first one, so
   the debug binds must be built from the operands of the first comparison and
   not from x compared with itself.  */

int
foo (int x, int y)
{
  signed char c;
  if (y == x)
    c = 0;
  else if (x < y)
    c = -1;
  else
    c = 1;
  return c > 0;
}

int
bar (double x, double y)
{
  signed char c;
  if (y == x)
    c = 0;
  else if (x < y)
    c = -1;
  else if (x > y)
    c = 1;
  else
    c = -128;
  return c > 0;
}

/* { dg-final { scan-tree-dump-not {DEBUG D#[0-9]+ => ([xy]_[0-9]+\(D\)) [<>=]+ \1 } "phiopt1" } } */
/* { dg-final { scan-tree-dump {DEBUG D#[0-9]+ => [xy]_[0-9]+\(D\) == [xy]_[0-9]+\(D\) } "phiopt1" } } */
