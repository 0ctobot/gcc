/* PR target/119980 */
/* { dg-do run } */
/* { dg-options "-O2 -fno-tree-fre" } */

/* With FRE off, it's DOM that needs to keep the stores.  */

#include "pr119980-2.c"
