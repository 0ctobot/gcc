/* PR tree-optimization/128265 */
/* { dg-do compile } */
/* { dg-options "-O1 -fdump-tree-phiprop1-details" } */

/* An int has no invalid values and fits in both objects, so this speculated
   load stays fine.  */

union u { int i; long long l; } x;
int other;

int
f (int c, int d)
{
  int *p = c ? &x.i : &other;
  if (d)
    return *p;
  return 0;
}

/* { dg-final { scan-tree-dump "Inserting PHI for result of load" "phiprop1" } } */
