/* PR tree-optimization/128213 */
/* { dg-do compile } */
/* { dg-options "-O1 -fdump-tree-phiprop1-details" } */

/* ch is smaller than the int load, which only runs when d is nonzero.  The
   load mustn't be speculated on the edge defining &ch.  */

_Alignas (int) char ch[1];
int iv;

int
f (int c, int d)
{
  int *p = c ? (int *) ch : &iv;
  if (d)
    return *p;
  return 0;
}

/* { dg-final { scan-tree-dump-not "Inserting PHI for result of load" "phiprop1" } } */
