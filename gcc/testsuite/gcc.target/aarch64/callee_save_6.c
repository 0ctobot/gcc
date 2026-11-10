/* { dg-do compile } */
/* { dg-options "-O2 -mtune=neoverse-n1" } */

/* Reduced from el1_interrupt() and el1_undef() in the Linux kernel's
   arch/arm64/kernel/entry-common.c.  The second argument is live across a
   call and regs already takes x19.  x20 pairs with x19, so saving it
   needs no extra instruction and the target prices its save and restore
   at 1 each.  That makes x20 cost the same as x1 with a caller save, and
   the tie should go to x20 since it doesn't need a store and a load around
   the call.  */

struct pt_regs { unsigned long regs[31], sp, pc, pstate; };

void enter_from_kernel_mode (struct pt_regs *);
void irq_enter_rcu (void);
void do_interrupt_handler (struct pt_regs *, void (*) (struct pt_regs *));
void exit_to_kernel_mode (struct pt_regs *, long);
void do_undefinstr (struct pt_regs *, unsigned long);

void
irq (struct pt_regs *regs, void (*handler) (struct pt_regs *))
{
  asm volatile ("msr daif, %0" :: "r" (0xc0UL));
  enter_from_kernel_mode (regs);
  irq_enter_rcu ();
  do_interrupt_handler (regs, handler);
  exit_to_kernel_mode (regs, regs->pstate);
}

void
undef (struct pt_regs *regs, unsigned long esr)
{
  enter_from_kernel_mode (regs);
  asm volatile ("msr daif, %0" :: "r" (regs->pstate & 0x3c0));
  do_undefinstr (regs, esr);
  asm volatile ("msr daifset, #0xf");
  exit_to_kernel_mode (regs, regs->regs[0]);
}

/* { dg-final { scan-assembler-not {\tstr\tx1, \[sp} } } */
/* { dg-final { scan-assembler-times {\tstp\tx19, x20, \[sp, [0-9]+\]} 2 } } */
