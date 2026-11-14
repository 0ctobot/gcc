/* An alias of a variable gets a location of its own, even when a function
   using it has been through points-to analysis.  */
/* { dg-do compile } */
/* { dg-require-alias "" } */
/* { dg-options "-O2 -gdwarf -dA" } */

int w = 1;
extern int w_alias __attribute__ ((alias ("w")));

int
f (void)
{
  w_alias = 2;
  return w;
}

/* { dg-final { scan-assembler-times " DW_AT_location" 2 } } */
/* { dg-final { scan-assembler "DW_OP_addr\[^\n\r\]*\[\n\r\]+\[^\n\r\]*w_alias" } } */
