/* PR ipa/128436 */
/* { dg-do run { target aarch64*-*-* } } */
/* { dg-options "-O3 -fno-ipa-sra -Wno-psabi --param=ipa-cp-eval-threshold=1 -fdump-ipa-cp-details" } */

/* _Float16 and __bf16 have the same size but different modes, so a constant of
   one loaded as the other keeps its bytes, alone and inside a vector.  */

#include "pr128436.h"

typedef short v4hi __attribute__ ((vector_size (8)));
typedef _Float16 v4hf __attribute__ ((vector_size (8)));
typedef __bf16 v4bf __attribute__ ((vector_size (8)));

TEST (t1, _Float16, __bf16, 0.75f16)
TEST (t2, __bf16, _Float16, 0.75f16)
TEST (t3, v4hf, v4bf, { 0.75f16, 2.5f16, 1.0f16, 2.0f16 })
TEST (t4, v4hi, v4hf, { 0x3e00, 0x4000, 1, 2 })
TEST (t5, v4hf, v4hi, { 0.75f16, 2.5f16, 1.0f16, 2.0f16 })

int
main (void)
{
  t1 ();
  t2 ();
  t3 ();
  t4 ();
  t5 ();
  return 0;
}

/* { dg-final { scan-ipa-dump-times "Modifying stmt" 10 "cp" } } */
