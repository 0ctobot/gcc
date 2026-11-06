/* { dg-do run } */
/* { dg-options "-O1 -mgeneral-regs-only" } */

/* Without SIMD registers, the 8-byte vectors need lowering too.  */

#include "../../gcc.dg/torture/pr126666-3.c"
