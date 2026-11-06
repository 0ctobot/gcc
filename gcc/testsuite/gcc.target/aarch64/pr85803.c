/* PR rtl-optimization/85803 */
/* { dg-do compile } */
/* { dg-options "-O2 -fno-tree-dse -fno-tree-sink -fno-store-merging -fno-tree-loop-im" } */

/* The RTL dead store elimination pass has to keep stores to memory visible
   outside of the function when the function may stop running before it gets
   to the store that kills them or to the end of the function.  */

extern void publish (int *);

int x, y;
volatile int v;

void
f1 (void)
{
  x = 42;
  __asm__ volatile ("");
  x = 0;
}

void
f2 (void)
{
  int l;
  publish (&l);
  l = 42;
  __builtin_trap ();
}

void
f3 (void)
{
  y = 42;
  v = 1;
  y = 0;
}

/* { dg-final { scan-assembler-times "mov\tw\[0-9\]+, 42" 3 } } */
