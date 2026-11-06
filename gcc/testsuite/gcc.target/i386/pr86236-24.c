/* PR target/86236 */
/* { dg-do compile { target ia32 } } */
/* { dg-options "-O2 -fno-pic -ffixed-edi -ffixed-esi -ffixed-ebx -ffixed-ecx -ffixed-edx" } */
/* AX is the only register left, and nothing is passed in it.  */
void u (void *);
volatile int n = 16;
void
f (int a)
{
  char *p = __builtin_alloca (n);
  char x[64] __attribute__ ((aligned (64)));
  u (p);
  u (x);
  u ((void *) a);
}
/* { dg-final { scan-assembler "leal\[ \t\]+4\\(%esp\\), %eax" } } */
