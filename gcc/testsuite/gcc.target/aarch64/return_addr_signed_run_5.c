/* Like return_addr_signed_run_2.c, but without FEAT_PAuth, so the return
   address is loaded from its save slot into X30 and stripped with
   XPACLRI, rather than copied into X30 from another register.  */
/* { dg-do run } */
/* { dg-options "-O2 -march=armv8-a -mbranch-protection=pac-ret -save-temps" } */

#include "return_addr_signed_run_2.c"

/* { dg-final { scan-assembler {\tldr\tx30, \[(?:x29|sp), [0-9]+\]\n\thint\t7 // xpaclri\n} } } */
/* { dg-final { scan-assembler-not {\tmov\tx30, x[0-9]+\n\thint\t7 // xpaclri\n} } } */
/* { dg-final { scan-assembler-not {\txpaci\t} } } */
