/* PR c/100529 */
/* { dg-do compile } */
/* { dg-options "-O0 -std=gnu11 -w -fdump-tree-gimple" } */

void bar ();

/* The declaration of z reads the temporary for its initializer, so the
   temporary's stack space can't be reclaimed until the block ends.  The only
   restore should be the one for the block.  Running this doesn't show the bug
   since z would land on top of the reclaimed temporary.  */
void
f (int n, int c)
{
  struct S { char a[n]; } x, y;
  __auto_type z = c ? x : y;
  bar (z);
}

/* { dg-final { scan-tree-dump-times "__builtin_stack_restore" 1 "gimple" } } */
