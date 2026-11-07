/* PR c/100529 */
/* { dg-do compile } */
/* { dg-options "-fgnu-tm -O2 -std=gnu23" } */

/* Where the sizes of a conditional's result come from doesn't depend on
   -fgnu-tm.  */

#include "../torture/pr100529-12.c"
