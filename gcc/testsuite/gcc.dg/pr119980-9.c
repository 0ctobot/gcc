/* PR target/119980 */
/* { dg-do run } */
/* { dg-options "-O2 -fno-tree-fre -fno-tree-dse -fno-tree-dominator-opts" } */

/* With FRE, DSE and DOM off, it's the elimination done by PRE that needs to
   keep the last store in each function.  */

#include "pr119980-6.c"
