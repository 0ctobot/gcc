/* { dg-do compile { target { lp64 && aarch64_little_endian } } } */
/* { dg-options "-O2" } */

/* A volatile read is one access of the element's size even when its neighbor
   isn't volatile, so load_pair_lanes must not widen the pair into a single
   load that covers both elements.  */

typedef long long v2di __attribute__ ((vector_size (16)));
typedef double v2df __attribute__ ((vector_size (16)));

v2di
mix_first (volatile long long *p)
{
  long long a = p[-1];
  long long b = ((long long *) p)[0];
  return (v2di) { a, b };
}

v2di
mix_second (long long *p)
{
  long long a = p[-1];
  long long b = *(volatile long long *) p;
  return (v2di) { a, b };
}

v2df
mix_first_df (volatile double *p)
{
  double a = p[-1];
  double b = ((double *) p)[0];
  return (v2df) { a, b };
}

v2df
mix_second_df (double *p)
{
  double a = p[-1];
  double b = *(volatile double *) p;
  return (v2df) { a, b };
}

/* { dg-final { scan-assembler-not {\mldr\tq[0-9]+, \[x0, -8\]} } } */
/* { dg-final { scan-assembler-times {\mldr\td[0-9]+, \[x0, -8\]} 4 } } */
/* { dg-final { scan-assembler-times {\mld1\t\{v[0-9]+\.d\}\[1\], \[x0\]} 4 } } */
