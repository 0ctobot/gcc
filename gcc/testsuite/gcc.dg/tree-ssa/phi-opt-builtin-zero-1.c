/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-optimized" } */
/* { dg-additional-options "-mbmi -mlzcnt" { target i?86-*-* x86_64-*-* } } */
/* { dg-final { scan-tree-dump-times "\\.CLZ" 3 "optimized" { target { { i?86-*-* x86_64-*-* aarch64-*-* } && lp64 } } } } */
/* { dg-final { scan-tree-dump-times "\\.CTZ" 1 "optimized" { target { { i?86-*-* x86_64-*-* aarch64-*-* } && lp64 } } } } */
/* { dg-final { scan-tree-dump-not "__builtin_c\[lt\]z" "optimized" { target { { i?86-*-* x86_64-*-* aarch64-*-* } && lp64 } } } } */
/* { dg-final { scan-tree-dump-not "PHI <" "optimized" { target { { i?86-*-* x86_64-*-* aarch64-*-* } && lp64 } } } } */

/* The zero check is redundant when evaluating the statements that use the
   builtin's result at its value for zero gives the other PHI argument.  */

int
fls32 (unsigned int x)
{
  return x ? 32 - __builtin_clz (x) : 0;
}

int
fls64 (unsigned long long x)
{
  if (!x)
    return 0;
  return 63 - __builtin_clzll (x) + 1;
}

int
ilog2 (unsigned int x)
{
  return x ? 31 - __builtin_clz (x) : -1;
}

int
ffs32 (unsigned int x)
{
  return x ? __builtin_ctz (x) + 1 : 33;
}
