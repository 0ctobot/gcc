/* PR target/119980 */
/* { dg-do run } */
/* { dg-options "-O2 -fno-tree-fre" } */

/* With FRE off, it's DSE that needs to keep the store of zero.  */

#include "pr119980-6.c"
