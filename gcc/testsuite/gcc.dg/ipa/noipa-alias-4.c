/* PR ipa/127509 */
/* Properties discovered from the body of a noipa function must not reach the
   callers of its aliases: pure-const sets them on the aliases even though it
   refuses to set them on the noipa function itself.  */
/* { dg-do compile } */
/* { dg-require-alias "" } */
/* { dg-options "-O2 -fdump-tree-optimized" } */

int g;

__attribute__ ((noipa)) int const_a (int v) { return v; }
extern int const_b (int) __attribute__ ((alias ("const_a")));

__attribute__ ((noipa)) int pure_a (void) { return g; }
extern int pure_b (void) __attribute__ ((alias ("pure_a")));

__attribute__ ((noipa)) void noreturn_a (void) { for (;;); }
extern void noreturn_b (void) __attribute__ ((alias ("noreturn_a")));

extern void *alloc_it (unsigned long) __attribute__ ((malloc));
__attribute__ ((noipa)) void *
malloc_a (unsigned long n)
{
  return alloc_it (n);
}
extern void *malloc_b (unsigned long) __attribute__ ((alias ("malloc_a")));

int
use_const (void)
{
  const_b (1);
  return 0;
}

int
use_pure (void)
{
  pure_b ();
  return 1;
}

int
use_noreturn (void)
{
  noreturn_b ();
  g = 5;
  return 7;
}

int
use_malloc (int *q)
{
  int *p = malloc_b (4);
  *q = 1;
  *p = 2;
  return *q;
}

/* { dg-final { scan-tree-dump "const_b \\(1\\)" "optimized" } } */
/* { dg-final { scan-tree-dump "pure_b \\(\\)" "optimized" } } */
/* { dg-final { scan-tree-dump "g = 5" "optimized" } } */
/* { dg-final { scan-tree-dump "= malloc_b \\(4\\)" "optimized" } } */
