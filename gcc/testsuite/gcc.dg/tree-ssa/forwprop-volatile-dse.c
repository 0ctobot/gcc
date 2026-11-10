/* The simple aggregate DSE in forwprop must not delete a copy into an
   unused local when the copy reads volatile memory.  */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-forwprop1-details -fdump-tree-optimized" } */

#define __READ_ONCE(x) (*(const volatile __typeof__ (x) *) &(x))
#define READ_ONCE(x) ({ __READ_ONCE (x); })

typedef struct { unsigned long pte; } pte_t;
struct irqstat { unsigned int cause; unsigned int mask; };
struct dev_regs { volatile struct irqstat stat; unsigned int ctl; };
struct big { long a, b, c, d, e, f, g, h; };

void
touch_pte (pte_t *ptep)
{
  (void) READ_ONCE (*ptep);
}

void
sample_pte (pte_t *ptep)
{
  pte_t old = READ_ONCE (*ptep);

  (void) old;
}

/* A read-to-clear register block declared with a volatile member.  */
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

struct big mkp (struct big) __attribute__ ((pure));

/* A pure call is deleted as a whole when its result is dead, which would
   take the volatile read of its argument with it.  */
void
touch_big_call (struct big *p)
{
  struct big w = mkp (*(volatile struct big *) p);

  (void) w;
}

/* Plain dead copies into an unused local are still removed.  */
void
plain_copy (struct big *p)
{
  struct big w = *p;

  (void) w;
}

void
plain_memcpy (struct big *p)
{
  struct big w;

  __builtin_memcpy (&w, p, sizeof (w));
  (void) w;
}

/* { dg-final { scan-tree-dump-not "Removing dead store stmt \[^\n\]*={v}" "forwprop1" } } */
/* { dg-final { scan-tree-dump-not "Removing dead call store stmt" "forwprop1" } } */
/* { dg-final { scan-tree-dump "mkp \\(MEM\[^\n\]*volatile struct big" "optimized" } } */
/* { dg-final { scan-tree-dump-times "Removing dead store stmt w = \\*p" 1 "forwprop1" } } */
/* { dg-final { scan-tree-dump-times "={v} (?:MEM|regs)" 4 "optimized" } } */
/* { dg-final { scan-tree-dump-not "__builtin_memcpy" "optimized" } } */
