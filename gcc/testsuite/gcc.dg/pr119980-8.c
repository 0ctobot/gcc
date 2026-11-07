/* PR target/119980 */
/* { dg-do run } */
/* { dg-options "-O2 -fno-tree-fre -fno-tree-dse" } */

/* With FRE and DSE off, it's DOM that needs to keep the last store in each
   function.  */

#include "pr119980-6.c"
