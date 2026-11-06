/* { dg-do run { target { aarch64_sve256_hw && mmap } } } */
/* { dg-options "-O3 -msve-vector-bits=256" } */

#include "pr128433.c"
