/* PR tree-optimization/128265 */
/* { dg-do compile } */
/* { dg-options "-O1 -fdump-tree-phiprop1-details" } */

/* The union doesn't hold a bool when d is zero, and a value outside [0, 1]
   loaded as _Bool is undefined.  The load mustn't be speculated.  */

union u { _Bool b; long long l; } x;
_Bool other;

_Bool
f (int c, int d)
{
  _Bool *p = c ? &x.b : &other;
  if (d)
    return *p;
  return 0;
}

/* { dg-final { scan-tree-dump-not "Inserting PHI for result of load" "phiprop1" } } */
