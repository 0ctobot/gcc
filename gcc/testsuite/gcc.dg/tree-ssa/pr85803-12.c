/* PR tree-optimization/85803 */
/* Single-arm conditional store replacement must not sink a store to global
   memory past a call that may not return.  */
/* { dg-do compile } */
/* { dg-options "-O2 -fallow-store-data-races -fdump-tree-phiopt1-details -fdump-tree-cselim-details" } */

int x;

__attribute__ ((noinline)) static int
spin (int c)
{
  while (c)
    ;
  return 0;
}

/* A store before the condition.  */
int
f1 (int c)
{
  int r = 0;
  x = 0;
  if (c)
    {
      x = 1;
      r = spin (c);
    }
  return r;
}

/* A load before the condition.  */
int
f2 (int c)
{
  int r = x;
  if (c)
    {
      x = 1;
      r += spin (c);
    }
  return r;
}

/* Neither, so the replacement needs to insert a load.  */
int
f3 (int c)
{
  int r = 0;
  if (c)
    {
      x = 1;
      r = spin (c);
    }
  return r;
}

/* { dg-final { scan-tree-dump-not "Conditional store replacement happened" "phiopt1" } } */
/* { dg-final { scan-tree-dump-not "Conditional store replacement happened" "cselim" } } */
