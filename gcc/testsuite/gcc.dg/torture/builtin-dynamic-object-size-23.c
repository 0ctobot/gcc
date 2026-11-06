/* builtin-dynamic-object-size-23.c at every optimization level, since its
   results depend on which objsz pass sizes each call.  At -O0 the builtin
   isn't folded and gives the maximum size.  */
/* { dg-do run } */
/* { dg-skip-if "not folded at -O0" { *-*-* } { "-O0" } { "" } } */

#include "../builtin-dynamic-object-size-23.c"
