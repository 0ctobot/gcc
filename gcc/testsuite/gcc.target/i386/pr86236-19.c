/* PR target/86236 */
/* { dg-do compile { target lp64 } } */
/* { dg-options "-O2 -mforce-drap -mstackrealign -ffixed-r10 -ffixed-rdi -ffixed-rsi -ffixed-r12 -ffixed-r13 -ffixed-r14 -ffixed-r15 -ffixed-rbx" } */

/* The stack allocation includes the outgoing argument area, and once it no
   longer fits in an immediate, the prologue adjusts the stack pointer through
   R11 before the argument pointer is copied out of the DRAP register.  Every
   other register is fixed or carries an argument or the return value, so no
   register is left for the DRAP.  */

struct B { char b[0x3ffffff0]; };
extern void t (struct B);
extern void g (void *);
extern long sa[];

__attribute__ ((ms_abi)) long
big (struct B *p, long n, long c3, long c4, long c5)
{
  sa[1] = c5 + c3 + c4;
  char l[0x3ffffef0];
  g (l);
  char x[64] __attribute__ ((aligned (64)));
  g (x);
  __asm__ volatile ("" : : : "xmm6", "xmm7", "xmm8", "xmm9", "xmm10", "xmm11",
			     "xmm12", "xmm13", "xmm14", "xmm15");
  t (*p);
  return n;
} /* { dg-message "no register is available" } */
