/* { dg-do compile } */
/* { dg-require-effective-target masm_intel } */
/* { dg-require-effective-target fpic } */
/* { dg-require-effective-target tls } */
/* { dg-options "-O2 -m16 -masm=intel -fPIC -mtls-dialect=gnu2 -ftls-model=global-dynamic" } */

/* The call through a TLS descriptor has its own output template.  */

extern __thread int v;

int
f1 (void)
{
  return v;
}

/* { dg-final { scan-assembler "call\[ \t\]+\\\[\[^\n\]*@TLSCALL" } } */
/* { dg-final { scan-assembler-not "call\[ \t\]+\\\[DWORD PTR" } } */
