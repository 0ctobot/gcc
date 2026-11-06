/* PR target/90346 */
/* A CPU with SSE2 keeps using LFENCE.  */
/* { dg-do compile } */
/* { dg-options "-O2 -m32 -march=pentium4" } */

int array[500];

int
f (unsigned int i)
{
  if (i < 500)
    return array[__builtin_speculation_safe_value (i)];
  return 0;
}

/* { dg-final { scan-assembler "lfence" } } */
/* { dg-final { scan-assembler-not "cpuid" } } */
