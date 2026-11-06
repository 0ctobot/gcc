/* PR tree-optimization/85803 */
/* { dg-do compile } */
/* { dg-options "-O2 -fno-tree-slp-vectorize -fno-schedule-insns -fno-schedule-insns2 -fno-schedule-fusion -fno-tree-ter" } */

/* Accesses must not be fused into a pair across an insn with a volatile
   reference, in either direction, even when the insn doesn't touch memory as
   far as the compiler can tell.  */

void
store_asm (long *p)
{
  p[0] = 1;
  __asm__ volatile ("");
  p[1] = 2;
}

void
store_unspec_volatile (long *p)
{
  p[0] = 1;
  __builtin_aarch64_set_fpcr (0);
  p[1] = 2;
}

void
store_volatile_read (long *p, volatile int *v)
{
  p[0] = 1;
  (void) *v;
  p[1] = 2;
}

long
load_asm (long *p)
{
  long a = p[0];
  __asm__ volatile ("" : : "r" (a));
  return a + p[1];
}

long
load_volatile_read (long *p, volatile int *v)
{
  long a = p[0];
  (void) *v;
  return a + p[1];
}

/* { dg-final { scan-assembler-not "\tstp\t" } } */
/* { dg-final { scan-assembler-not "\tldp\t" } } */
