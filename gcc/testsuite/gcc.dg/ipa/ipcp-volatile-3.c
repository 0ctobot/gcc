/* { dg-do compile } */
/* { dg-require-alias "" } */
/* { dg-options "-O2 -fipa-cp-clone -fno-ipa-icf -fno-ipa-sra --param ipa-cp-eval-threshold=1 -fdump-tree-optimized" } */

/* The aggregate read from volatile memory is passed through an alias of the
   callee, so IPA-CP must look at the callers of the alias as well when it
   decides whether the clone can drop the unused parameter.  */

struct word { unsigned long value; };

volatile struct word vword;
extern void g (int);

static __attribute__ ((noinline)) void
ignore (struct word w, int k)
{
  for (int i = 0; i < k; i++)
    g (i);
}

static void ignore_alias (struct word, int) __attribute__ ((alias ("ignore")));

void
read_through_alias (void)
{
  ignore_alias (vword, 5);
}

/* { dg-final { scan-tree-dump-not "\\.constprop \\(\\)" "optimized" } } */
/* { dg-final { scan-tree-dump "ignore\\.constprop \\(vword\\)" "optimized" } } */
