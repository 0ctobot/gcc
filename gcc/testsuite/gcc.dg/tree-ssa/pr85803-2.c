/* PR tree-optimization/85803 */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-sink1-details -fdump-tree-sink2-details" } */

/* A store to memory that's visible outside of the function mustn't sink past a
   branch to an endless loop, since the store then never happens on that path
   even though the program stays around to observe its absence.  */

extern void g (void);

int a1, a2, a3, a4, a5, y;

void
f1 (int e, int c)
{
  if (e)
    {
      a1 = 42;
      if (c)
	for (;;);
    }
  else
    y = 1;
}

void
f2 (int c)
{
  a2 = 42;
  if (c)
    for (;;);
  g ();
}

int
f3 (int c)
{
  a3 = 42;
  if (c)
    for (;;);
  return a3;
}

void
f4 (int d, int c)
{
  if (d)
    {
      a4 = 42;
      if (c)
	for (;;)
	  y = 1;
    }
  else
    g ();
}

/* The two stores mustn't be commoned into the join block either.  */
void
f5 (int a, int c)
{
  if (a)
    {
      a5 = 1;
      if (c)
	for (;;);
    }
  else
    a5 = 2;
}

/* { dg-final { scan-tree-dump-not "Sinking \[^\n\]*\n *a\[1-4\] = 42" "sink1" } } */
/* { dg-final { scan-tree-dump-not "Sinking \[^\n\]*\n *a\[1-4\] = 42" "sink2" } } */
/* { dg-final { scan-tree-dump-not "sinking common stores to a5" "sink1" } } */
/* { dg-final { scan-tree-dump-not "sinking common stores to a5" "sink2" } } */
