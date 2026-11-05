/* { dg-add-options vect_early_break } */
/* { dg-require-effective-target vect_early_break_hw } */
/* { dg-require-effective-target vect_int } */

#include "tree-vect.h"

/* The only exit that the compiler can count is DC reaching null, so the
   segment of DC ends at the top of the address space and its end wraps to a
   small address.  The first store changes *SC, so the run-time alias test must
   not let this loop be vectorized when SC is just above DC.  */

__attribute__ ((noipa)) void
f (char *sc, short *dc)
{
  while (dc && (*dc++ = *sc))
    ;
}

int
main (void)
{
  short a[64];

  check_vect ();

  __builtin_memset (a, 1, sizeof (a));

  /* Point at the byte of a[0] that becomes 0 once a[0] is 1.  */
  f ((char *) a + (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__), a);
  if (a[0] != 1 || a[1] != 0 || a[2] != 257)
    __builtin_abort ();
  return 0;
}
