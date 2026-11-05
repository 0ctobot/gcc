/* PR tree-optimization/128265 */
/* { dg-do compile } */
/* { dg-options "-O1 -fdump-tree-phiprop1-details" } */

/* The _Bool load stays within tt, which is why checking the size of the load
   isn't enough: tt only holds a valid _Bool when d is nonzero.  The speculated
   loads must read the whole byte and truncate it to a _Bool.  */

long long tt;
_Bool other;

_Bool
f (int c, int d)
{
  _Bool *p = c ? (_Bool *) &tt : &other;
  if (d)
    return *p;
  return 0;
}

/* { dg-final { scan-tree-dump "Inserting PHI for result of load" "phiprop1" } } */
/* Match the conversions in the function body, not in the details output.  */
/* { dg-final { scan-tree-dump-times "  _\[0-9\]+ = \\((?:_Bool|bool)\\) _\[0-9\]+;" 2 "phiprop1" } } */
