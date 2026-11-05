/* { dg-do compile { target { lp64 && aarch64_little_endian } } } */
/* { dg-options "-O2" } */

/* Each volatile read below is one access of the element's size.  Combine
   must not fold the two reads into one wider load through load_pair_lanes,
   which would read both elements with a single access.  */

typedef double v2df __attribute__ ((vector_size (16)));
typedef float v2sf __attribute__ ((vector_size (8)));
typedef int v2si __attribute__ ((vector_size (8)));

v2df
both_df (volatile double *p)
{
  double a = p[-1];
  double b = p[0];
  return (v2df) { a, b };
}

v2sf
both_sf (volatile float *p)
{
  float a = p[-1];
  float b = p[0];
  return (v2sf) { a, b };
}

v2si
both_si (volatile int *p)
{
  int a = p[-1];
  int b = p[0];
  return (v2si) { a, b };
}

/* { dg-final { scan-assembler-not {\mldr\tq[0-9]+, \[x0, -8\]} } } */
/* { dg-final { scan-assembler-not {\mldr\td[0-9]+, \[x0, -4\]} } } */
/* { dg-final { scan-assembler-times {\mldr\td[0-9]+, \[x0, -8\]} 1 } } */
/* { dg-final { scan-assembler-times {\mldr\ts[0-9]+, \[x0, -4\]} 2 } } */
/* { dg-final { scan-assembler-times {\mld1\t\{v[0-9]+\.d\}\[1\], \[x0\]} 1 } } */
/* { dg-final { scan-assembler-times {\mld1\t\{v[0-9]+\.s\}\[1\], \[x0\]} 2 } } */
