/* PR c/100529 */
/* { dg-do compile } */
/* { dg-additional-options "-std=gnu23" } */

/* Where the sizes of a conditional's result come from doesn't depend on
   -fopenmp.  */

#include "../torture/pr100529-12.c"
