/* { dg-do compile } */
/* { dg-skip-if "" { *-*-* } { "-mcpu=*" "-march=*" "-mtune=*" } { "" } } */
/* { dg-options "-O3 -mcpu=neoverse-n2" } */

/* Same as absdiff_subs_1.c, but with early scheduling enabled and with a
   tuning that fuses CMP with CSEL, which keeps the compare after both
   subtractions.  */

#include "absdiff_subs_1.c"

/* { dg-final { scan-assembler-times {\tsubs\tx[0-9]+, x[0-9]+, x[0-9]+\n} 3 } } */
/* { dg-final { scan-assembler-times {\tsubs\tw[0-9]+, w[0-9]+, w[0-9]+\n} 2 } } */
/* { dg-final { scan-assembler-not {\tcmp\t} } } */
