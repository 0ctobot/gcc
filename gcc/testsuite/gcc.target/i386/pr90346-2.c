/* PR target/90346 */
/* The i486 has neither LFENCE nor, in early models, CPUID.  */
/* { dg-do compile } */
/* { dg-options "-O2 -m32 -march=i486" } */

int array[500];

int
f (unsigned int i)
{
  if (i < 500)
    return array[__builtin_speculation_safe_value (i)];
  return 0;
}

/* { dg-final { scan-assembler-not "lfence" } } */
/* { dg-final { scan-assembler-not "cpuid" } } */
/* { dg-final { scan-assembler "lock" } } */
