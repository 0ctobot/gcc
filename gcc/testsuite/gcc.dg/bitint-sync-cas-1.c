/* On targets that extend the padding bits of a _BitInt, the __sync fetch
   builtins on a _BitInt with padding bits are expanded into a loop around
   __sync_val_compare_and_swap.  The swap succeeds only if the returned value
   is the old value passed to it, so the loop must compare it with the old
   value and not with the new one.  */
/* { dg-do compile { target { bitint && { arm*-*-* loongarch*-*-* riscv*-*-* s390*-*-* } } } } */
/* { dg-options "-std=c23 -fdump-tree-original" } */

unsigned _BitInt(17) x;

unsigned _BitInt(17)
fetch_add (unsigned _BitInt(17) v)
{
  return __sync_fetch_and_add (&x, v);
}

unsigned _BitInt(17)
add_fetch (unsigned _BitInt(17) v)
{
  return __sync_add_and_fetch (&x, v);
}

/* { dg-final { scan-tree-dump "(D\\.\\d+) = \[^\n\]*__sync_val_compare_and_swap_4 \[^\n\]*TARGET_EXPR <(D\\.\\d+), \[^\n\]*\\), \\1 == TARGET_EXPR <\\2, " "original" } } */
/* { dg-final { scan-tree-dump-not "__sync_val_compare_and_swap_4 \[^\n\]* (D\\.\\d+)\\), D\\.\\d+ == \\1\\)" "original" } } */
