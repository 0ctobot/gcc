/* { dg-do run } */
/* { dg-additional-options "-ftrivial-auto-var-init=pattern -Wno-uninitialized" } */
/* { dg-require-effective-target indirect_jumps } */
/* { dg-require-effective-target label_values } */
/* { dg-require-effective-target nonlocal_goto } */

#include "auto-init-self-7.c"
