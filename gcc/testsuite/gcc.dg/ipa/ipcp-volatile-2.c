/* { dg-do compile } */
/* { dg-options "-O2 -fipa-cp-clone -fno-ipa-icf -fno-ipa-sra --param ipa-cp-eval-threshold=1 -fdump-ipa-cp-details -fdump-tree-optimized" } */

/* Reading a const volatile variable doesn't produce a known constant, even
   when the variable has an initializer, so IPA-CP must neither propagate the
   variable into a clone nor drop the read when the callee ignores the
   parameter.  An argument read from ordinary memory can still be dropped.  */

struct word { unsigned long value; };

extern const volatile struct word cvw_ext;
const volatile struct word cvw = { 3 };
struct word plain;
extern void g (int);

static __attribute__ ((noinline)) void
ignore_1 (struct word w, int k)
{
  for (int i = 0; i < k; i++)
    g (i);
}

static __attribute__ ((noinline)) void
ignore_2 (struct word w, int k)
{
  for (int i = 0; i < k; i++)
    g (i);
}

static __attribute__ ((noinline)) void
ignore_plain (struct word w, int k)
{
  for (int i = 0; i < k; i++)
    g (i);
}

void
read_extern (void)
{
  ignore_1 (cvw_ext, 5);
}

void
read_initialized (void)
{
  ignore_2 (cvw, 6);
}

void
read_plain (void)
{
  ignore_plain (plain, 7);
}

/* { dg-final { scan-ipa-dump-times "UNKNOWN, volatile read" 2 "cp" } } */
/* { dg-final { scan-tree-dump "ignore_1\\.constprop \\(cvw_ext\\)" "optimized" } } */
/* { dg-final { scan-tree-dump "ignore_2\\.constprop \\(cvw\\)" "optimized" } } */
/* { dg-final { scan-tree-dump "ignore_plain\\.constprop \\(\\)" "optimized" } } */
