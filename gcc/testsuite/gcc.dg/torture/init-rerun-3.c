/* { dg-do run } */
/* { dg-options "-ftrivial-auto-var-init=pattern" } */
/* { dg-require-effective-target indirect_jumps } */

/* Padding of a variable with an initializer is zero under the pattern mode as
   well.  */

#include "init-rerun-2.c"
