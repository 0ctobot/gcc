/* PR tree-optimization/128213 */
/* { dg-do compile } */
/* { dg-options "-O1 -fdump-tree-phiprop1-details" } */

/* Same as pr128213-2.c, except the four bytes all lie within both objects,
   so the load can be speculated.  */

char buf[8];
int iv;

int
f (int c, int d)
{
  char *p = c ? &buf[4] : (char *) &iv;
  if (d)
    {
      int v;
      __builtin_memcpy (&v, p, sizeof (v));
      return v;
    }
  return 0;
}

/* { dg-final { scan-tree-dump "Inserting PHI for result of load" "phiprop1" } } */
