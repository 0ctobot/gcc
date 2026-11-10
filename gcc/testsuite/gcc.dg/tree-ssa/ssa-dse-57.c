/* An asm statement only accesses a local whose address is never taken
   through its own operands.  The zero initialization of b is dead in the
   f_ functions, where an asm with a "memory" clobber or a memory operand
   for another object sits between it and the copy that overwrites b.  In
   the k_ functions, the asm reads b or gets its address, so the
   initialization has to stay.  */
/* { dg-do compile } */
/* { dg-options "-O2 -ftrivial-auto-var-init=zero -fdump-tree-dse1-details -fdump-tree-optimized" } */

struct big { long a[8]; };

long
f_barrier (const struct big *src, int i)
{
  struct big b;
  __asm__ __volatile__ ("" : : : "memory");
  b = *src;
  return b.a[i & 7];
}

long
f_regs (const struct big *src, int i)
{
  struct big b;
  long t;
  __asm__ __volatile__ ("" : "=r" (t) : "r" (i) : "memory");
  b = *src;
  return b.a[t & 7];
}

long
f_other_out (const struct big *src, int *cnt, int i)
{
  struct big b;
  __asm__ __volatile__ ("" : "+m" (*cnt));
  b = *src;
  return b.a[i & 7];
}

long
f_other_in (const struct big *src, const int *cnt, int i)
{
  struct big b;
  __asm__ __volatile__ ("" : : "m" (*cnt));
  b = *src;
  return b.a[i & 7];
}

long
k_in (const struct big *src, int i)
{
  struct big b;
  __asm__ __volatile__ ("" : : "m" (b));
  b = *src;
  return b.a[i & 7];
}

long
k_addr (const struct big *src, int i)
{
  struct big b;
  __asm__ __volatile__ ("" : : "r" (&b) : "memory");
  b = *src;
  return b.a[i & 7];
}

/* { dg-final { scan-tree-dump-times "Deleted dead store: b = \\.DEFERRED_INIT" 4 "dse1" } } */
/* { dg-final { scan-tree-dump-times "\\.DEFERRED_INIT" 2 "optimized" } } */
