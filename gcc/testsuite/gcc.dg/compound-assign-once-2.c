/* Same as compound-assign-once-1.c, with the automatic temporaries of the
   compound literals in the index expressions initialized.  */
/* { dg-do run } */
/* { dg-options "-std=gnu11 -O2 -ftrivial-auto-var-init=zero" } */

#include "compound-assign-once-1.c"
