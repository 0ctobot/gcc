/* PR target/86236 */
/* { dg-do run { target lp64 } } */
/* { dg-options "-O2 -mgeneral-regs-only -mno-red-zone -ffixed-r12 -ffixed-r13 -ffixed-r14 -ffixed-r15 -ffixed-rbx" } */

/* A no_caller_saved_registers function has to preserve every register, but
   the registers that can hold a return value stay call-used in it, and the
   prologue doesn't save the incoming value of a call-used DRAP register.  The
   DRAP register therefore can't be DI, SI or DX here.  */

__attribute__ ((no_caller_saved_registers, noipa)) void
u (void *a, void *b)
{
  __asm__ volatile ("" : : "r" (a), "r" (b));
}

volatile int n = 16;

__attribute__ ((no_caller_saved_registers, noipa)) void
f (void)
{
  char *p = __builtin_alloca (n);
  char a[64] __attribute__ ((aligned (64)));
  u (p, a);
}

int
main (void)
{
  register long di __asm__ ("rdi") = 0x1111;
  register long si __asm__ ("rsi") = 0x2222;
  register long dx __asm__ ("rdx") = 0x3333;
  register long cx __asm__ ("rcx") = 0x4444;
  register long r8 __asm__ ("r8") = 0x5555;
  register long r9 __asm__ ("r9") = 0x6666;
  register long r10 __asm__ ("r10") = 0x7777;
  register long r11 __asm__ ("r11") = 0x8888;
  register long ax __asm__ ("rax") = 0x9999;
  __asm__ volatile ("call f"
		    : "+r" (di), "+r" (si), "+r" (dx), "+r" (cx), "+r" (r8),
		      "+r" (r9), "+r" (r10), "+r" (r11), "+r" (ax)
		    : : "memory", "cc");
  if (di != 0x1111 || si != 0x2222 || dx != 0x3333 || cx != 0x4444
      || r8 != 0x5555 || r9 != 0x6666 || r10 != 0x7777 || r11 != 0x8888
      || ax != 0x9999)
    __builtin_abort ();
  return 0;
}
