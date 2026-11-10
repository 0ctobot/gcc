/* { dg-do compile } */
/* { dg-options "-O2 -mtune=neoverse-n1 -fno-shrink-wrap-separate" } */

/* Without separate shrink-wrapping, every callee-saved register is saved at
   the function's entry, so the cost of a callee-saved register for part
   and p doesn't change.  Both of them are then saved around the call in
   the loop along with done and to.  */

#include "callee_save_4.c"

/* { dg-final { scan-assembler-times {\t(?:ld|st)[rp]\tx(?:1[0-8]|[0-9]), (?:x[0-9]+, )?\[sp, [0-9]+\]} 5 } } */
