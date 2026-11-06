/* PR tree-optimization/125696 */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-optimized" } */

/* Each volatile read of *p must survive.  Rewriting the partial stores into
   v as BIT_INSERT_EXPRs must not turn the reads into plain loads, which FRE
   would then merge into one.  */

typedef int v4si __attribute__((vector_size (16)));
struct S { int i; };

v4si
f (volatile struct S *p)
{
  v4si v = { 1, 2, 3, 4 };
  *(struct S *) &v = *p;
  *(struct S *) &v = *p;
  return v;
}

/* { dg-final { scan-tree-dump-times "={v}" 2 "optimized" } } */
