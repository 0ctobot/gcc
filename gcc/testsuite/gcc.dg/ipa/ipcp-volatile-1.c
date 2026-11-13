/* { dg-do compile } */
/* { dg-options "-O2 -fipa-cp-clone -fno-ipa-icf -fno-ipa-sra --param ipa-cp-eval-threshold=1 -fdump-ipa-cp-details -fdump-tree-optimized" } */

/* Each caller reads an aggregate argument from volatile memory and passes it
   to a parameter that the callee never uses.  IPA-CP clones the callees for
   the constant in the second parameter, but the clones must keep the first
   one, or the calls redirected to them lose the volatile reads.  */

struct word { unsigned long value; };
struct holder { volatile struct word w; };

volatile struct word vword;
struct holder holder;
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
ignore_3 (struct word w, int k)
{
  for (int i = 0; i < k; i++)
    g (i);
}

void
read_through_pointer (struct word *p)
{
  ignore_1 (*(const volatile struct word *) p, 5);
}

void
read_through_pointer_twice (struct word *p, int k)
{
  ignore_1 (*(const volatile struct word *) p, 5);
  ignore_1 (*(const volatile struct word *) p, k);
}

void
read_volatile_var (void)
{
  ignore_2 (vword, 6);
}

void
read_volatile_member (void)
{
  ignore_3 (holder.w, 7);
}

/* { dg-final { scan-ipa-dump-times "UNKNOWN, volatile read" 5 "cp" } } */
/* { dg-final { scan-tree-dump-not "\\.constprop \\(\\)" "optimized" } } */
/* { dg-final { scan-tree-dump-times "ignore_1\\.constprop \\(MEM\\\[\\(const volatile struct word \\*\\)p_\[0-9\]+\\(D\\)\\\]\\)" 2 "optimized" } } */
/* { dg-final { scan-tree-dump "ignore_2\\.constprop \\(vword\\)" "optimized" } } */
/* { dg-final { scan-tree-dump "ignore_3\\.constprop \\(holder\\.w\\)" "optimized" } } */
