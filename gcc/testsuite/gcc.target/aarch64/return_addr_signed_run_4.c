/* Like return_addr_signed_run_2.c, but signing with the B key, optimizing
   for size and probing the stack, which changes how the large frames and
   the alloca in big_fp are allocated.  */
/* { dg-do run { target aarch64*-*-linux* } } */
/* { dg-options "-Os -march=armv8.3-a -mbranch-protection=pac-ret+b-key -fstack-clash-protection -save-temps" } */
/* { dg-require-effective-target lp64 } */

#include "return_addr_signed_run_2.c"

/* { dg-final { scan-assembler {\txpaci\t} } } */
/* { dg-final { scan-assembler-not {xpaclri} } } */
