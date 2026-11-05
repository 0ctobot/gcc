/* PR ipa/128436 */
/* { dg-do run } */
/* { dg-options "-O3 -fno-ipa-sra -Wno-psabi --param=ipa-cp-eval-threshold=1 -fdump-ipa-cp-details" } */

/* A float vector constant loaded as an integer vector keeps its bytes.  */

#include "pr128436.h"

typedef int v4si __attribute__ ((vector_size (16)));
typedef unsigned int v4usi __attribute__ ((vector_size (16)));
typedef float v4sf __attribute__ ((vector_size (16)));
typedef long long v2di __attribute__ ((vector_size (16)));
typedef double v2df __attribute__ ((vector_size (16)));

TEST (t1, v4sf, v4si, { 0.7f, 2.5f, 1.0f, 2.0f })
TEST (t2, v4sf, v4usi, { 0.7f, -0.7f, 1.0f, 2.0f })
TEST (t3, v2df, v2di, { 0.7, 1.5 })

int
main (void)
{
  t1 ();
  t2 ();
  t3 ();
  return 0;
}

/* { dg-final { scan-ipa-dump-times "Modifying stmt" 6 "cp" { target aarch64*-*-* } } } */
