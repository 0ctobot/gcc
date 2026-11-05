/* PR tree-optimization/128265 */
/* { dg-do compile } */
/* { dg-options "-O1 -fdump-tree-phiprop1-details" } */

/* The _Bool load runs on every path out of the PHI, so moving it onto the
   incoming edges doesn't introduce a read the program didn't already do, and
   the loads don't need truncating.  */

long long tt;
_Bool other;

_Bool
f (int c)
{
  _Bool *p = c ? (_Bool *) &tt : &other;
  return *p;
}

/* { dg-final { scan-tree-dump "Inserting PHI for result of load" "phiprop1" } } */
/* { dg-final { scan-tree-dump-not "= \\((?:_Bool|bool)\\) _\[0-9\]+;" "phiprop1" } } */
