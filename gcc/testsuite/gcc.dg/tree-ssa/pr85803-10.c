/* PR tree-optimization/85803 */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-dse1-details" } */

/* A call that can't touch memory can't fault on it either, and neither can one
   that only gets the address of a named object or goes through the same pointer
   as the store, so these stores are still dead.  */

int g;
char buf[8];

__attribute__ ((const)) int cf (int);

int
f1 (int x)
{
  g = 1;
  x = cf (x);
  g = 2;
  return x;
}

void
f2 (void)
{
  g = 1;
  __builtin_memset (buf, 0, sizeof (buf));
  g = 2;
}

/* { dg-final { scan-tree-dump-times "Deleted dead store" 2 "dse1" } } */
