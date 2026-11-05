/* PR tree-optimization/128213 */
/* { dg-do compile } */
/* { dg-options "-O1 -fdump-tree-phiprop1-details" } */

/* &buf[3] leaves one byte in the object, and the memcpy reads four of them
   only when d is nonzero.  The load mustn't be speculated on that edge.  */

char buf[4];
int iv;

int
f (int c, int d)
{
  char *p = c ? &buf[3] : (char *) &iv;
  if (d)
    {
      int v;
      __builtin_memcpy (&v, p, sizeof (v));
      return v;
    }
  return 0;
}

/* { dg-final { scan-tree-dump-not "Inserting PHI for result of load" "phiprop1" } } */
