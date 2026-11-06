/* PR tree-optimization/128312 */
/* { dg-do run } */
/* { dg-options "-O2" } */
/* { dg-require-effective-target indirect_jumps } */

/* The callee stores to the return slot and then does a longjmp back to the
   caller, so the assignment in main never happens and D keeps its old value.
   The struct is long-aligned for targets on which a local struct of ints gets
   a larger alignment than the result, which disables the NRV in F.  */

#include <setjmp.h>

struct S { long a[16]; };
struct S g;
jmp_buf env;

__attribute__((noipa)) void
opaque (void)
{
  longjmp (env, 1);
}

__attribute__((noinline)) struct S
f (struct S *p)
{
  struct S r = *p;
  opaque ();
  return r;
}

int
main (void)
{
  struct S d;
  g.a[0] = 42;
  d.a[0] = 0;
  d.a[1] = 1;
  if (!setjmp (env))
    d = f (&g);
  if (d.a[0] != 0)
    __builtin_abort ();
  return 0;
}
