/* PR tree-optimization/128266 */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-optimized" } */

/* A relaxed atomic load from a volatile object is an observable access even
   when its value is unused.  */

volatile _Atomic int r2;
volatile int r3;

void
f (void)
{
  __atomic_load_n (&r2, __ATOMIC_RELAXED);
}

void
g (volatile int *p)
{
  __atomic_load_n (p, __ATOMIC_RELAXED);
}

void
h (void)
{
  __atomic_load_n (&r3, __ATOMIC_RELAXED);
}

/* { dg-final { scan-tree-dump-times "__atomic_load_4" 3 "optimized" } } */
