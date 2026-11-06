/* PR target/86236 */
/* { dg-do compile { target ia32 } } */
/* { dg-options "-O2 -fno-pic -ffixed-edi -ffixed-esi -ffixed-ebx" } */

/* A fastcall function without arguments isn't using CX for anything.  */

void u (void *, void *);

__attribute__ ((fastcall)) void
f (void)
{
  char *a = __builtin_alloca (32);
  int x __attribute__ ((aligned (64)));
  u (a, &x);
}

/* { dg-final { scan-assembler "leal\[ \t\]+\[0-9\]+\\(%esp\\), %ecx" } } */
