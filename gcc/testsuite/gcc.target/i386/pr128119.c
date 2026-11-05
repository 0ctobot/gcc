/* PR rtl-optimization/128119 */
/* { dg-do compile { target int128 } } */
/* { dg-options "-O2 -msse2" } */

typedef double v2df __attribute__ ((vector_size (16)));

unsigned long
a (volatile unsigned long *p, v2df m)
{
  unsigned long c = (unsigned long) __builtin_ia32_comisdeq (m, m) | 1;
  unsigned long v = *p;
  return ((unsigned __int128) v * c) >> 64;
}

unsigned long
b (unsigned long *p, v2df m)
{
  unsigned long c = (unsigned long) __builtin_ia32_comisdeq (m, m) | 1;
  unsigned long v = __atomic_load_n (p, __ATOMIC_RELAXED);
  return ((unsigned __int128) v * c) >> 64;
}

/* Both loads have to stay.  */
/* { dg-final { scan-assembler-times {\(%rdi\)} 2 } } */
