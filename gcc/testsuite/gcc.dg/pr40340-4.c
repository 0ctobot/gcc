/* PR middle-end/40340 */
/* { dg-do compile } */
/* { dg-options "-O2 -Wall -Wno-array-bounds -Wno-system-headers -g" } */

#define TEST3
#include "pr40340.h"

int
main (void)
{
  char buf[4];
  test3 (buf);
  return 0;
}

/* The overflowing call is inlined into main, which isn't in a system header,
   so the overflow is diagnosed despite -Wno-system-headers.  */
/* { dg-warning "\\\[-Wstringop-overflow" "" { target *-*-* } 10 } */
/* { dg-message "file included" "In file included" { target *-*-* } 0 } */
