/* { dg-do compile } */
/* { dg-options "-O2 -fdump-ipa-sra-details -fdump-tree-optimized" } */

/* Only the parameter fed by a volatile read has to stay intact.  The other
   aggregate parameter of the same function can still be split, and a
   parameter that is never used can still be removed.  */

struct word { unsigned long value; };
struct pair { unsigned first, second; };

static __attribute__ ((noinline)) unsigned
use_second (struct word w, struct pair q, int unused)
{
  return q.first;
}

unsigned
caller (struct word *p, struct pair *q, int k)
{
  return use_second (*(const volatile struct word *) p, *q, k);
}

/* { dg-final { scan-ipa-dump-times "is loaded from volatile memory, not removing" 1 "sra" } } */
/* { dg-final { scan-ipa-dump "Will split parameter 1" "sra" } } */
/* { dg-final { scan-ipa-dump "Will remove parameter 2" "sra" } } */
/* { dg-final { scan-ipa-dump-not "Will remove parameter 0" "sra" } } */
/* { dg-final { scan-ipa-dump-not "Will split parameter 0" "sra" } } */
