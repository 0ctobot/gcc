/* PR rtl-optimization/119980 */
/* { dg-do run } */
/* { dg-options "-O2 -fno-tree-fre -fno-tree-dominator-opts" } */

/* With FRE and DOM off, it's the elimination done by PRE that needs to keep the
   stores.  */

#include "pr119980-2.c"
