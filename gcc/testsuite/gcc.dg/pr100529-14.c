/* PR c/100529 */
/* { dg-do run } */
/* { dg-options "-std=gnu23 -fopenmp-simd" } */

/* Where the sizes of a conditional's result come from doesn't depend on
   -fopenmp-simd.  */

#include "torture/pr100529-12.c"
