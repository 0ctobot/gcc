/* PR tree-optimization/85803 */
/* { dg-do compile } */
/* { dg-options "-O2 -fno-tree-dse -fdump-tree-forwprop1" } */

/* The copy into B isn't dead just because a clobber of B comes after it, since
   the function may spin on VZ forever with B available through publish ().  */

struct S { int a[8]; };
extern void publish (struct S *);
struct S src;
volatile int vz;

void
f1 (void)
{
  struct S b;
  publish (&b);
  b = src;
  while (vz)
    ;
}

/* Nothing can see C, so the copy into it can go.  */

void
f2 (void)
{
  struct S c;
  c = src;
  while (vz)
    ;
}

/* { dg-final { scan-tree-dump "b = src" "forwprop1" } } */
/* { dg-final { scan-tree-dump-not "c = src" "forwprop1" } } */
