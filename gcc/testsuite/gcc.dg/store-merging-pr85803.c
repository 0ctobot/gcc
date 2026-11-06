/* PR tree-optimization/85803 */
/* { dg-do compile } */
/* { dg-require-effective-target store_merge } */
/* { dg-options "-O2 -fno-tree-dse -fno-tree-slp-vectorize -fdump-tree-store-merging" } */

/* Store merging moves the first store of a group down to the last one.  That
   isn't valid across a statement that the function may never get past, even
   though the statement doesn't read the stored memory.  */

struct S { char a, b; };
struct S s1, s2;

__attribute__ ((noinline)) static void
hang (int c)
{
  if (c)
    for (;;);
}

void
f1 (void)
{
  s1.a = 1;
  __asm__ volatile ("");
  s1.b = 2;
}

void
f2 (int c)
{
  s2.a = 1;
  hang (c);
  s2.b = 2;
}

/* { dg-final { scan-tree-dump-not "Merging successful" "store-merging" } } */
