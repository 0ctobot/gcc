/* An alias of a variable with the aligned attribute must not be assumed to
   be better aligned than the variable's storage.  Section anchors let the
   increase_alignment pass raise the alignment of the storage itself, which
   makes such a fold correct, so turn them off.  */
/* { dg-do run } */
/* { dg-require-alias "" } */
/* { dg-options "-O2 -fno-section-anchors -fdump-tree-optimized" } */

struct s { long x[9]; };

struct s w1 __attribute__ ((aligned (8))) = { { 1 } };
struct s w2 __attribute__ ((aligned (8))) = { { 2 } };
char w3[16] __attribute__ ((aligned (1))) = { 3 };
extern struct s a1 __attribute__ ((alias ("w1")));
extern struct s a2 __attribute__ ((alias ("w2")));
extern char a3[16] __attribute__ ((alias ("w3")));

__attribute__ ((noipa)) __UINTPTR_TYPE__
opaque (__UINTPTR_TYPE__ x)
{
  return x;
}

__attribute__ ((noipa)) void
check (__UINTPTR_TYPE__ x, __UINTPTR_TYPE__ y)
{
  if (x != y)
    __builtin_abort ();
}

int
main (void)
{
  check ((__UINTPTR_TYPE__) &a1 & 15, opaque ((__UINTPTR_TYPE__) &a1) & 15);
  check ((__UINTPTR_TYPE__) &a2 & 15, opaque ((__UINTPTR_TYPE__) &a2) & 15);
  check ((__UINTPTR_TYPE__) a3 & 7, opaque ((__UINTPTR_TYPE__) a3) & 7);
  return 0;
}

/* { dg-final { scan-tree-dump-not "check \\(0, " "optimized" } } */
