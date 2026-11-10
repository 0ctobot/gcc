/* { dg-lto-do run } */
/* { dg-skip-if "uses aarch64 assembly" { ! aarch64*-*-* } } */
/* { dg-require-effective-target elf } */
/* { dg-lto-options {{-O2 -flto -fcall-saved-x9}} } */
/* { dg-suppress-ld-options {-fcall-saved-x9} } */

/* Both files are compiled with -fcall-saved-x9, and the second one also with
   -fcall-used-x9 -fcall-saved-9, which names x9 by its register number and
   leaves it call-saved.  Every file treats x9 as call-saved, so the link-time
   compiler must keep it that way and make clobber save and restore x9.
   call_check sets x9, calls clobber and returns whether x9 survived.  */

__asm__ (".text\n"
	 ".p2align 2\n"
	 ".globl call_check\n"
	 ".type call_check, %function\n"
	 "call_check:\n"
	 "\tstp x29, x30, [sp, -16]!\n"
	 "\tmov x9, #0x123\n"
	 "\tbl clobber\n"
	 "\tcmp x9, #0x123\n"
	 "\tcset w0, eq\n"
	 "\tldp x29, x30, [sp], 16\n"
	 "\tret\n"
	 ".size call_check, .-call_check\n");

extern int call_check (void);
extern long saved (long);

__attribute__ ((noipa, used)) void
clobber (void)
{
  __asm__ volatile ("mov x9, #0" : : : "x9");
}

int
main (void)
{
  if (saved (2) != 3 || !call_check ())
    __builtin_abort ();
  return 0;
}
