/* { dg-do compile } */
/* { dg-require-effective-target masm_intel } */
/* { dg-require-effective-target fpic } */
/* { dg-require-effective-target tls } */
/* { dg-require-profiling "-pg" } */
/* { dg-options "-O2 -m16 -masm=intel -fPIC -fno-plt -ftls-model=global-dynamic -pg" } */

/* The calls to the TLS helper and to mcount and the sibcall through the GOT
   have their own output templates.  */

extern __thread int v;
extern void h (void);

int
f1 (void)
{
  return v;
}

void
f2 (void)
{
  h ();
}

/* { dg-final { scan-assembler "call\[ \t\]+\\\[\[^\n\]*tls_get_addr\[^\n\]*@GOT" } } */
/* { dg-final { scan-assembler "call\[ \t\]+\\\[\[^\n\]*mcount\[^\n\]*@GOT" } } */
/* { dg-final { scan-assembler "jmp\[ \t\]+\\\[\[^\n\]*h\[^\n\]*@GOT" } } */
/* { dg-final { scan-assembler-not "(?:jmp|call)\[ \t\]+\\\[DWORD PTR" } } */
