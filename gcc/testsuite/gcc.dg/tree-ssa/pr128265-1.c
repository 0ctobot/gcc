/* PR tree-optimization/128265 */
/* { dg-do compile } */
/* { dg-options "-O1 -fdump-tree-phiprop1-details" } */

/* The union doesn't hold a bool when d is zero, and a value outside [0, 1]
   loaded as _Bool is undefined.  The speculated loads must read the whole
   byte and truncate it to a _Bool.  */

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

/* { dg-final { scan-tree-dump "Inserting PHI for result of load" "phiprop1" } } */
/* Match the conversions in the function body, not in the details output.  */
/* { dg-final { scan-tree-dump-times "  _\[0-9\]+ = \\((?:_Bool|bool)\\) _\[0-9\]+;" 2 "phiprop1" } } */
