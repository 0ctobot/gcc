/* PR tree-optimization/79619 */
/* { dg-do compile } */
/* { dg-options "-O2 -masm=att" } */

/* Address zero is valid in these address spaces, so the pointers loaded from
   there point to real memory and the stores through them must stay.  */

void
f1 (void)
{
  *(*(char * __seg_fs *) 0) = 1;
}

void
f2 (void)
{
  *(*(char * __seg_gs *) 0) = 1;
}

/* The same goes for a zero that's only found in an aggregate.  */

struct S { char * __seg_fs *p; };

void
f3 (int i)
{
  struct S a[2] = {};
  **a[i].p = 1;
}

/* { dg-final { scan-assembler-times "mov\[lq\]\[ \t\]+%\[fg\]s:0," 3 } } */
/* { dg-final { scan-assembler-times "movb\[ \t\]+\\\$1, \\(%\[a-z0-9\]+\\)" 3 } } */
