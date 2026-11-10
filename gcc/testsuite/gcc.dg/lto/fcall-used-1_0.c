/* { dg-lto-do run } */
/* { dg-skip-if "uses aarch64 assembly" { ! aarch64*-*-* } } */
/* { dg-require-effective-target elf } */
/* { dg-lto-options {{-O2 -flto -fcall-used-x19}} } */
/* { dg-suppress-ld-options {-fcall-used-x19} } */

/* The file is compiled with -fcall-used-x19 but the link command line
   doesn't have it.  The link-time compiler must still not keep a value in
   x19 across the call to clobber_x19, which doesn't preserve it.  */

__asm__ (".text\n"
	 ".p2align 2\n"
	 ".type clobber_x19, %function\n"
	 "clobber_x19:\n"
	 "\tmov x19, #0\n"
	 "\tret\n"
	 ".size clobber_x19, .-clobber_x19\n");

extern void clobber_x19 (void);

int calls;
volatile long six = 6;

__attribute__ ((noipa)) long
keep (long a, int n)
{
  long t = a * 7, s = 0;
  do
    {
      clobber_x19 ();
      calls++;
      s += t;
    }
  while (--n > 0);
  return s;
}

int
main (void)
{
  if (keep (six, 3) != 126 || calls != 3)
    __builtin_abort ();
  return 0;
}
