/* PR tree-optimization/128213 */
/* { dg-do compile } */
/* { dg-options "-O1 -fdump-tree-phiprop1-details" } */

/* The load runs on every path out of the PHI, so it already happens in the
   original program regardless of its size, and moving it isn't speculation.  */

_Alignas (int) char ch[1];
int iv;

int
f (int c)
{
  int *p = c ? (int *) ch : &iv;
  return *p;
}

/* { dg-final { scan-tree-dump "Inserting PHI for result of load" "phiprop1" } } */
