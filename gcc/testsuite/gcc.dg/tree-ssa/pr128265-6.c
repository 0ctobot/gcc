/* PR tree-optimization/128265 */
/* { dg-do compile } */
/* { dg-options "-O1 -fdump-tree-phiprop1-details -fdump-tree-optimized" } */

/* Speculating the _Bool loads as truncated byte loads lets a and b stay out
   of memory.  */

int
f (int c, int d, _Bool a, _Bool b)
{
  _Bool *p = c ? &a : &b;
  if (d)
    return *p;
  return 0;
}

/* { dg-final { scan-tree-dump "Inserting PHI for result of load" "phiprop1" } } */
/* { dg-final { scan-tree-dump-not "&a|&b" "optimized" } } */
