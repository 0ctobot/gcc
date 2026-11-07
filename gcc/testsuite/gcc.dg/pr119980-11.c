/* PR target/119980 */
/* { dg-do run } */
/* { dg-options "-O2 -fno-tree-fre" } */

/* With FRE off, it's the elimination done by PRE that needs to keep the int
   store.  */

#include "pr119980-10.c"
