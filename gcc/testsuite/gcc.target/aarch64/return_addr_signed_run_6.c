/* Like return_addr_signed_run_2.c, but with the shadow call stack in X18
   and without FP and SIMD registers, as in an arm64 kernel build.  The
   cases that use X18 or leave through longjmp are skipped.  */
/* { dg-do run } */
/* { dg-options "-O2 -march=armv8.3-a -mgeneral-regs-only -mbranch-protection=pac-ret -fsanitize=shadow-call-stack -ffixed-x18 -DSCS -save-temps" } */

#include "return_addr_signed_run_2.c"

/* { dg-final { scan-assembler {\txpaci\t} } } */
/* { dg-final { scan-assembler-not {xpaclri} } } */
