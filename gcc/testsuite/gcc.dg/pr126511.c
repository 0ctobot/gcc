/* PR sanitizer/126511 */
/* { dg-do compile } */
/* { dg-options "-O1 -fsanitize=null -w -fdump-tree-ubsan" } */

struct A { };
void foo ();
int z;
__attribute__((returns_twice, const)) int bar (struct A);

void
baz (void)
{
  struct A *x;
  int *y;
  if (z)
    foo ();
  *y = bar (*x);
}

/* The block split for the returns_twice call must not get the call instrumented
   a second time.  */
/* { dg-final { scan-tree-dump-times "\\.UBSAN_NULL \\(y" 1 "ubsan" } } */
/* { dg-final { scan-tree-dump-times "\\.UBSAN_NULL \\(x" 1 "ubsan" } } */
