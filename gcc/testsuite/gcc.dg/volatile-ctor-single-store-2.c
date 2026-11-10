/* A lone constant vector fills the volatile struct by itself, but most of
   its lanes are zero, which can make GCC clear the target before storing
   the vector.  The target must still be stored to once.  */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-optimized" } */

typedef unsigned char v16qi __attribute__ ((vector_size (16)));
struct w { v16qi v; };

void
f (volatile struct w *p)
{
  *p = (struct w) { { 1 } };
}

/* { dg-final { scan-tree-dump-times "={v}" 1 "optimized" } } */
