/* PR ipa/128436 */
/* { dg-do run } */
/* { dg-options "-O3 -fno-ipa-sra -Wno-psabi --param=ipa-cp-eval-threshold=1 -fdump-ipa-cp-details" } */

/* An integer vector constant loaded as a float vector keeps its bytes, rather
   than having each element converted to the float of the same value.  */

#include "pr128436.h"

typedef int v4si __attribute__ ((vector_size (16)));
typedef unsigned int v4usi __attribute__ ((vector_size (16)));
typedef float v4sf __attribute__ ((vector_size (16)));
typedef long long v2di __attribute__ ((vector_size (16)));
typedef double v2df __attribute__ ((vector_size (16)));

TEST (t1, v4si, v4sf, { 0x3f333333, 0x40000000, 3, 1 })
TEST (t2, v4usi, v4sf, { 0xbf333333, 0x40000000, 3, 1 })
TEST (t3, v2di, v2df, { 0x3fe6666666666666LL, 1 })

int
main (void)
{
  t1 ();
  t2 ();
  t3 ();
  return 0;
}

/* { dg-final { scan-ipa-dump-times "Modifying stmt" 6 "cp" { target aarch64*-*-* } } } */
