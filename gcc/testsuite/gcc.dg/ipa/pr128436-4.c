/* PR ipa/128436 */
/* { dg-do run } */
/* { dg-require-effective-target dfp } */
/* { dg-options "-O3 -fno-ipa-sra -Wno-psabi --param=ipa-cp-eval-threshold=1 -fdump-ipa-cp-details" } */

/* A decimal float constant loaded as a binary float keeps its bytes, and so
   does a binary float constant loaded as a decimal float.  */

#include "pr128436.h"

TEST (t1, _Decimal32, float, 1.5DF)
TEST (t2, float, _Decimal32, 0.7f)
TEST (t3, _Decimal64, double, 1.5DD)
TEST (t4, double, _Decimal64, 0.7)

int
main (void)
{
  t1 ();
  t2 ();
  t3 ();
  t4 ();
  return 0;
}

/* { dg-final { scan-ipa-dump-times "Modifying stmt" 8 "cp" { target aarch64*-*-* } } } */
