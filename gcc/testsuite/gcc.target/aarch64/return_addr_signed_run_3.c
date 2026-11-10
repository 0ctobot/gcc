/* Like return_addr_signed_run_2.c, but with leaf functions signed too and
   without a frame pointer, so the return address slot is addressed from
   the stack pointer even in the large frame of big_sp.  */
/* { dg-do run } */
/* { dg-options "-O3 -march=armv8.3-a -mbranch-protection=pac-ret+leaf -fomit-frame-pointer -save-temps" } */

#include "return_addr_signed_run_2.c"

/* { dg-final { scan-assembler {\txpaci\t} } } */
/* { dg-final { scan-assembler-not {xpaclri} } } */
