/* { dg-lto-do link } */
/* { dg-lto-options { { -O2 -flto -fipa-cp-clone -fno-ipa-icf -fno-ipa-sra --param ipa-cp-eval-threshold=1 -fdump-ipa-cp-details -fdump-tree-optimized } } } */

/* Both calls read the argument for the parameter that ignore_word never uses
   from volatile memory.  That has to survive the streaming of the jump
   functions, so that the clone IPA-CP creates for the constant in the second
   parameter keeps the first one and the redirected calls keep the reads.  */

struct word { unsigned long value; };
extern volatile struct word vword;
extern void ignore_word (struct word w, int k);

int
main (void)
{
  ignore_word (vword, 5);
  ignore_word (*(const volatile struct word *) &vword, 5);
  return 0;
}

/* { dg-final { scan-wpa-ipa-dump-times "UNKNOWN, volatile read" 2 "cp" } } */
/* { dg-final { scan-wpa-ipa-dump "Creating a specialized node of ignore_word" "cp" } } */
/* { dg-final { scan-ltrans-tree-dump-not "ignore_word\\.constprop \\(\\)" "optimized" } } */
