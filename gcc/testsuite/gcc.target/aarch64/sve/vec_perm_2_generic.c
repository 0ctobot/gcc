/* { dg-do compile } */
/* { dg-options "-O2 -msve-vector-bits=2048" } */

typedef unsigned char v256qi __attribute__ ((vector_size (256)));

/* A byte selector can only refer to the first vector at VL2048, so X must be
   the only input to the permutation.  */
void
f (v256qi *r, const v256qi *x, const v256qi *y, const v256qi *z)
{
  *r = __builtin_shuffle (*x, *y, *z);
}

/* { dg-final { scan-assembler-times {\ttbl\tz[0-9]+\.b, \{z[0-9]+\.b\}, z[0-9]+\.b\n} 1 } } */
/* { dg-final { scan-assembler-not {\torr\t} } } */
