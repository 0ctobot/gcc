/* Volatile aggregate reads into an unused local still load memory, each
   with its full width.  */
/* { dg-do compile } */
/* { dg-options "-O2 -mgeneral-regs-only" } */

#define __READ_ONCE(x) (*(const volatile __typeof__ (x) *) &(x))
#define READ_ONCE(x) ({ __READ_ONCE (x); })

typedef struct { __UINT64_TYPE__ pte; } pte_t;
struct irqstat { unsigned int cause; unsigned int mask; };
struct dev_regs { volatile struct irqstat stat; unsigned int ctl; };
struct big { __INT64_TYPE__ a, b, c, d, e, f, g, h; };

void
sample_pte (pte_t *ptep)
{
  pte_t old = READ_ONCE (*ptep);

  (void) old;
}

void
ack_irq (struct dev_regs *regs)
{
  struct irqstat s = regs->stat;

  (void) s;
}

void
touch_big (struct big *p)
{
  struct big w = *(volatile struct big *) p;

  (void) w;
}

/* { dg-final { scan-assembler-times "\tldr\tx\[0-9\]+, \\\[x0\\\]\n" 2 } } */
/* { dg-final { scan-assembler-times "\tldp\tx\[0-9\]+, x\[0-9\]+, \\\[x0(?:, \[0-9\]+)?\\\]\n" 4 } } */
