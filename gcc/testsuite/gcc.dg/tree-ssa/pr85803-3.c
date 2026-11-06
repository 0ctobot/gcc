/* PR tree-optimization/85803 */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-lim2-details" } */

/* Store motion delays a store in a loop until the loop exits.  That doesn't
   work when the loop has no exit or when something in the loop may keep the
   function from ever reaching the exit.  */

int a1, a2, a3, a4, a5;
volatile int v;

__attribute__ ((noinline)) static void
hang (int c)
{
  if (c)
    for (;;);
}

void
f1 (void)
{
  for (;;)
    a1 = 42;
}

void
f2 (int n, int c)
{
  for (int i = 0; i < n; i++)
    {
      a2 = i;
      hang (c);
    }
}

void
f3 (int n)
{
  for (int i = 0; i < n; i++)
    {
      a3 = i;
      __asm__ volatile ("");
    }
}

void
f4 (void)
{
  while (v)
    a4 = 1;
}

void
f5 (int n)
{
  for (int i = 0; i < n; i++)
    {
      a5 = i;
      while (v)
	;
    }
}

/* A counted loop cannot spin on the volatile access, so this store may wait for
   the loop to end.  */
int a6;

void
f6 (int n)
{
  for (int i = 0; i < n; i++)
    {
      a6 = i;
      v = i;
    }
}

/* { dg-final { scan-tree-dump-not "Executing store motion of a\[1-5\]" "lim2" } } */
/* { dg-final { scan-tree-dump "Executing store motion of a6" "lim2" } } */
