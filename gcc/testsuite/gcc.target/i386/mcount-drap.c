/* Without -mfentry, mcount is called after the prologue loads the DRAP
   register.  mcount may clobber the call-used registers that can't carry an
   argument, so the DRAP register must not be one of them.  my_mcount
   clobbers R10 and R11, which used to hold the DRAP register.  */
/* { dg-do run { target { lp64 && *-*-linux* } } } */
/* { dg-require-profiling "-pg" } */
/* { dg-options "-O2 -pg -mno-fentry -fno-shrink-wrap" } */

__asm__ (".text\n"
	 ".globl my_mcount\n"
	 ".type my_mcount, @function\n"
	 "my_mcount:\n"
	 "\tmovq $-1, %r10\n"
	 "\tmovq $-1, %r11\n"
	 "\tret\n"
	 ".size my_mcount, .-my_mcount\n");

#define ATTR __attribute__ ((noipa, fentry_name ("my_mcount")))

ATTR long
f1 (long a, long b, long c, long d, long e, long f, long g, long h)
{
  char x[64] __attribute__ ((aligned (64)));
  char v[a];
  __asm__ volatile ("" : : "r" (x), "r" (v) : "memory");
  return a + b + c + d + e + f + g * 7 + h * 8;
}

ATTR __attribute__ ((preserve_none)) long
f2 (long a, long b, long c, long d, long e, long f, long g, long h)
{
  char x[64] __attribute__ ((aligned (64)));
  char v[a];
  __asm__ volatile ("" : : "r" (x), "r" (v) : "memory");
  return a + b + c + d + e + f + g * 7 + h * 8;
}

int
main (void)
{
  if (f1 (1, 2, 3, 4, 5, 6, 7, 8) != 134
      || f2 (1, 2, 3, 4, 5, 6, 7, 8) != 134)
    __builtin_abort ();
  return 0;
}
