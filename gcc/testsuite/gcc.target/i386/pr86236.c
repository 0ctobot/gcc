/* PR target/86236 */
/* { dg-do compile { target ia32 } } */
/* { dg-options "-O2 -mstackrealign -msse2" } */

typedef long long __m128i __attribute__ ((vector_size (16)));
void f1 (void *, int);
register int edi __asm__ ("edi");

__attribute__ ((fastcall)) void
f2 (void)
{
  __m128i m;
  f1 (&m, edi);
}

/* The argument pointer must not clobber the global register variable.  */
/* { dg-final { scan-assembler-not "leal\[ \t\]+\[0-9\]+\\(%esp\\), %edi" } } */
/* { dg-final { scan-assembler "leal\[ \t\]+\[0-9\]+\\(%esp\\), %esi" } } */
