/* PR target/85939 */
/* { dg-do compile { target ia32 } } */
/* { dg-options "-O2 -mstackrealign -mno-mmx -mno-sse" } */

typedef int v2 __attribute__ ((vector_size (8)));

int f1 (v2 *);

int
f2 (void)
{
  v2 v;
  return f1 (&v);
}

/* The local needs 8-byte alignment, which -mstackrealign can't assume from the
   caller.  */
/* { dg-final { scan-assembler "andl\[ \t\]+\\\$-\[0-9\]+, %esp" } } */
