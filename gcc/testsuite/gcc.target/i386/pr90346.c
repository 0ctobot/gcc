/* PR target/90346 */
/* LFENCE needs SSE2, so CPUs without it need a different speculation
   barrier.  */
/* { dg-do compile } */
/* { dg-options "-O2 -m32 -march=pentium2" } */

/* CPUID overwrites this register, so the barrier has to preserve it.  */
register int g asm ("ebx");

int array[500];

int
f (unsigned int i)
{
  if (i < 500)
    return array[__builtin_speculation_safe_value (i)];
  return 0;
}

/* { dg-final { scan-assembler-not "lfence" } } */
/* { dg-final { scan-assembler-times "cpuid" 1 } } */
/* { dg-final { scan-assembler-times "pushal" 1 } } */
/* { dg-final { scan-assembler-times "popal" 1 } } */
