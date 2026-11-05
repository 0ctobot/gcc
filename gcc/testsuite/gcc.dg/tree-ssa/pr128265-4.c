/* PR tree-optimization/128265 */
/* { dg-do compile } */
/* { dg-options "-O1 -std=gnu23 -fdump-tree-phiprop1-details" } */
/* { dg-require-effective-target bitint } */

/* Values outside of the range of a _BitInt(3) are undefined too, even though
   the bytes holding them are readable.  */

union u { unsigned _BitInt(3) b; long long l; } x;
unsigned _BitInt(3) other;

unsigned _BitInt(3)
f (int c, int d)
{
  unsigned _BitInt(3) *p = c ? &x.b : &other;
  if (d)
    return *p;
  return 0;
}

/* { dg-final { scan-tree-dump-not "Inserting PHI for result of load" "phiprop1" } } */
