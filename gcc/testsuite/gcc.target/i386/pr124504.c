/* { dg-do compile } */
/* { dg-require-effective-target masm_intel } */
/* { dg-options "-O2 -m16 -masm=intel" } */

/* In 16-bit mode, the assembler takes DWORD PTR on an indirect jump or call to
   mean a far pointer.  */

void
f1 (void (*func) (void))
{
  func ();
}

int
f2 (void (*func) (void))
{
  func ();
  return 1;
}

/* { dg-final { scan-assembler "jmp\[ \t\]+\\\[" } } */
/* { dg-final { scan-assembler "call\[ \t\]+\\\[" } } */
/* { dg-final { scan-assembler-not "(?:jmp|call)\[ \t\]+\\\[DWORD PTR" } } */
