/* PR tree-optimization/85803 */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-optimized" } */

/* Stores to memory that's visible outside of the function have to stay when
   the function may never get as far as the store that kills them.  */

extern void publish (int *);

int y;
volatile int vz;

/* A path to an endless loop next to a path to the kill.  */
int a1;
void
f1 (int c)
{
  a1 = 42;
  if (c)
    for (;;);
  a1 = 0;
}

/* The endless loop has a virtual definition.  */
int a2;
void
f2 (int c)
{
  a2 = 42;
  if (c)
    for (;;)
      y = 1;
  a2 = 0;
}

/* The endless loop has no way out for the walk to follow.  */
int a3;
void
f3 (void)
{
  a3 = 42;
  for (;;)
    y = 1;
}

/* A volatile asm in an endless loop.  */
int a4;
void
f4 (void)
{
  a4 = 42;
  for (;;)
    __asm__ volatile ("");
}

/* An asm goto to an endless loop.  */
int a5;
void
f5 (void)
{
  a5 = 42;
  __asm__ goto ("" : : : : spin);
  a5 = 0;
  return;
spin:
  for (;;);
}

/* Killed on the path through the branch and through nothing else.  */
int a6;
void
f6 (int a, int c)
{
  if (a)
    {
      a6 = 42;
      if (c)
	for (;;);
      a6 = 3;
    }
  else
    a6 = 2;
}

/* A callee that doesn't touch memory and doesn't always return.  */
__attribute__ ((noinline)) static void
hang1 (int c)
{
  if (c)
    for (;;);
}

int a7;
void
f7 (int c)
{
  a7 = 42;
  hang1 (c);
  a7 = 0;
}

/* The same with a volatile asm in the callee.  */
__attribute__ ((noinline)) static void
hang2 (int c)
{
  if (c)
    for (;;)
      __asm__ volatile ("");
}

int a8;
void
f8 (int c)
{
  a8 = 42;
  hang2 (c);
  a8 = 0;
}

/* The callee reads a volatile, which isn't a use of a variable that can't be
   seen from the callee or a different type of variable.  */
__attribute__ ((noinline)) static void
hang3 (int c)
{
  while (c && !vz);
}

static int a9;
int
read_a9 (void)
{
  return a9;
}

void
f9 (int c)
{
  a9 = 42;
  hang3 (c);
  a9 = 0;
}

float a10;
void
f10 (int c)
{
  a10 = 42;
  hang3 (c);
  a10 = 0;
}

/* Spin loops that have an exit but aren't assumed to finish.  */
int a11;
void
f11 (int c)
{
  a11 = 42;
  while (c)
    __asm__ volatile ("");
  a11 = 0;
}

int a12;
void
f12 (void)
{
  a12 = 42;
  while (!vz);
  a12 = 0;
}

/* A volatile asm can stop in there.  */
int a13;
void
f13 (void)
{
  a13 = 42;
  __asm__ volatile ("");
  a13 = 0;
}

/* Local variables that something else got the address of.  */
void
f14 (void)
{
  int l14;
  publish (&l14);
  l14 = 42;
  for (;;);
}

void
f15 (int c)
{
  int l15;
  publish (&l15);
  l15 = 42;
  if (c)
    for (;;);
  l15 = 0;
  publish (0);
}

void
f16 (void)
{
  int l16;
  publish (&l16);
  l16 = 42;
  for (;;)
    __asm__ volatile ("");
}

void
f17 (void)
{
  int l17;
  publish (&l17);
  l17 = 42;
  while (!vz);
}

void
f18 (void)
{
  int l18;
  publish (&l18);
  l18 = 42;
  __asm__ volatile ("");
}

void
f19 (int c)
{
  int l19;
  publish (&l19);
  l19 = 42;
  hang1 (c);
}

/* A store that's overwritten inside of the endless loop is still dead.  */
int a20;
void
f20 (void)
{
  a20 = 42;
  for (;;)
    a20 = 1;
}

/* Nothing can stop at a __builtin_unreachable.  */
int a21;
void
f21 (int c)
{
  a21 = 42;
  if (c)
    __builtin_unreachable ();
  a21 = 0;
}

/* An integer that escapes makes points-to analysis say that escaped memory can
   be anything, but nobody can see a local whose address is never taken.  */
extern void hook (long *);
extern volatile long vl;
long
f22 (int i)
{
  long pages22[4];
  long avail22 = vl;
  pages22[i] = avail22;
  avail22 += vl;
  hook (&avail22);
  return avail22;
}

/* A volatile access that only runs once cannot get stuck, even in a local
   variable that something else got the address of.  */
int a23;
void
f23 (void)
{
  a23 = 42;
  vz = 1;
  a23 = 0;
}

void
f24 (void)
{
  int l24;
  publish (&l24);
  l24 = 42;
  vz = 1;
  l24 = 0;
  publish (0);
}

/* A goto makes a volatile access run more than once too, including when the
   cycle has more than one way in.  */
int a25;
void
f25 (void)
{
  a25 = 42;
again:
  if (!vz)
    goto again;
  a25 = 0;
}

int a26;
void
f26 (int c)
{
  a26 = 42;
  if (c)
    goto second;
first:
  if (vz)
    goto out;
second:
  if (vz != 2)
    goto first;
out:
  a26 = 0;
}

/* { dg-final { scan-tree-dump "a1 = 42" "optimized" } } */
/* { dg-final { scan-tree-dump "a2 = 42" "optimized" } } */
/* { dg-final { scan-tree-dump "a3 = 42" "optimized" } } */
/* { dg-final { scan-tree-dump "a4 = 42" "optimized" } } */
/* { dg-final { scan-tree-dump "a5 = 42" "optimized" } } */
/* { dg-final { scan-tree-dump "a6 = 42" "optimized" } } */
/* { dg-final { scan-tree-dump "a7 = 42" "optimized" } } */
/* { dg-final { scan-tree-dump "a8 = 42" "optimized" } } */
/* { dg-final { scan-tree-dump "a9 = 42" "optimized" } } */
/* { dg-final { scan-tree-dump "a10 = 4" "optimized" } } */
/* { dg-final { scan-tree-dump "a11 = 42" "optimized" } } */
/* { dg-final { scan-tree-dump "a12 = 42" "optimized" } } */
/* { dg-final { scan-tree-dump "a13 = 42" "optimized" } } */
/* { dg-final { scan-tree-dump "l14 = 42" "optimized" } } */
/* { dg-final { scan-tree-dump "l15 = 42" "optimized" } } */
/* { dg-final { scan-tree-dump "l16 = 42" "optimized" } } */
/* { dg-final { scan-tree-dump "l17 = 42" "optimized" } } */
/* { dg-final { scan-tree-dump "l18 = 42" "optimized" } } */
/* { dg-final { scan-tree-dump "l19 = 42" "optimized" } } */
/* { dg-final { scan-tree-dump-not "a20 = 42" "optimized" } } */
/* { dg-final { scan-tree-dump-not "a21 = 42" "optimized" } } */
/* { dg-final { scan-tree-dump-not "pages22\\\[" "optimized" } } */
/* { dg-final { scan-tree-dump-not "a23 = 42" "optimized" } } */
/* { dg-final { scan-tree-dump-not "l24 = 42" "optimized" } } */
/* { dg-final { scan-tree-dump "a25 = 42" "optimized" } } */
/* { dg-final { scan-tree-dump "a26 = 42" "optimized" } } */
