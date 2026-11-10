/* { dg-lto-do run } */
/* { dg-skip-if "uses aarch64 assembly" { ! aarch64*-*-* } } */
/* { dg-require-effective-target elf } */
/* { dg-lto-options {{-O2 -flto -ffixed-x19 -fcall-saved-x19}} } */
/* { dg-suppress-ld-options {-ffixed-x19 -fcall-saved-x19} } */

/* The file is compiled with -ffixed-x19 followed by -fcall-saved-x19, so
   the later option wins and x19 is an ordinary call-saved register.  The
   link command line has neither option.  The link-time compiler must apply
   them in the same order, and so make clobber save and restore x19.
   call_check sets x19, calls clobber and returns whether x19 survived.  */

__asm__ (".text\n"
	 ".p2align 2\n"
	 ".globl call_check\n"
	 ".type call_check, %function\n"
	 "call_check:\n"
	 "\tstp x29, x30, [sp, -32]!\n"
	 "\tstr x19, [sp, 16]\n"
	 "\tmov x19, #0x123\n"
	 "\tbl clobber\n"
	 "\tcmp x19, #0x123\n"
	 "\tcset w0, eq\n"
	 "\tldr x19, [sp, 16]\n"
	 "\tldp x29, x30, [sp], 32\n"
	 "\tret\n"
	 ".size call_check, .-call_check\n");

extern int call_check (void);

__attribute__ ((noipa, used)) void
clobber (void)
{
  __asm__ volatile ("mov x19, #0" : : : "x19");
}

int
main (void)
{
  if (!call_check ())
    __builtin_abort ();
  return 0;
}
