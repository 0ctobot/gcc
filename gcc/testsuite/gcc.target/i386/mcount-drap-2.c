/* A function without caller-saved registers treats every register that can't
   hold a return value as call-saved, but mcount may still clobber the
   registers that are call-used in the function's ABI, like R10 and R11.  With
   R12-R15 and RBX fixed and the first two argument registers live, R10 used to
   be picked as the DRAP register and was clobbered by my_mcount before the
   body read the stack argument through it.  */
/* { dg-do run { target { lp64 && *-*-linux* } } } */
/* { dg-require-profiling "-pg" } */
/* { dg-options "-O2 -pg -mno-fentry -mgeneral-regs-only -fno-shrink-wrap -ffixed-r12 -ffixed-r13 -ffixed-r14 -ffixed-r15 -ffixed-rbx" } */

__asm__ (".text\n"
	 ".globl my_mcount\n"
	 ".type my_mcount, @function\n"
	 "my_mcount:\n"
	 "\tmovq $-1, %r10\n"
	 "\tmovq $-1, %r11\n"
	 "\tret\n"
	 ".size my_mcount, .-my_mcount\n");

struct big
{
  long x[4];
};

__attribute__ ((noipa, no_caller_saved_registers,
		fentry_name ("my_mcount"))) long
f (long a, long b, struct big s)
{
  char x[64] __attribute__ ((aligned (64)));
  char v[a];
  __asm__ volatile ("" : : "r" (x), "r" (v) : "memory");
  return a + b + s.x[0] * 3 + s.x[1] * 4 + s.x[2] * 5 + s.x[3] * 6;
}

int
main (void)
{
  struct big s = { { 3, 4, 5, 6 } };
  if (f (1, 2, s) != 89)
    __builtin_abort ();
  return 0;
}

/* { dg-final { cleanup-profile-file } } */
