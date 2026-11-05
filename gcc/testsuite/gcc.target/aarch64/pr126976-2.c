/* PR tree-optimization/126976 */
/* { dg-do compile } */
/* { dg-options "-O2 -Wno-psabi" } */

/* Truncation keeps the low bits whatever the sign of its source, so mixed-sign
   extracts can still share one narrowing conversion.  */

typedef signed char v8qi __attribute__ ((vector_size (8)));
typedef short v8hi __attribute__ ((vector_size (16)));
typedef unsigned short v8uhi __attribute__ ((vector_size (16)));

v8qi
narrow (v8hi a)
{
  v8uhi au = (v8uhi) a;
  return (v8qi) { au[0], a[1], au[2], a[3], au[4], a[5], au[6], a[7] };
}

/* { dg-final { scan-assembler-times {\txtn\tv[0-9]+\.8b, v[0-9]+\.8h} 1 } } */
/* { dg-final { scan-assembler-not {\t(?:umov|smov|ins)\t} } } */
