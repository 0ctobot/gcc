/* PR rtl-optimization/127847 */
/* { dg-do run } */
/* { dg-options "-O3 -ftrivial-auto-var-init=zero -fdump-rtl-hoist" } */
/* { dg-final { scan-rtl-dump-times "copying expression" 1 "hoist" } } */

/* The Linux kernel reads user memory with an asm goto that has an output and
   branches to a label on a fault.  Code hoisting only runs in functions that
   are optimized for size, like the kernel's __init functions, which are cold.
   Loading zero stands in for a fault here to take the label at run time.  */
#define get_user_asm(x, addr, label)				\
  asm volatile goto ("ldr\t%x0, [%1]\n\tcbz\t%x0, %l2"		\
		     : "=r" (x) : "r" (addr) : : label)

/* V + 100 is computed on both of the asm goto's paths, but it needs the value
   written by the asm goto, so it cannot be hoisted above it.  */

__attribute__((noipa, cold)) long
both_paths (unsigned long *p)
{
  unsigned long v;

  get_user_asm (v, p, fault);
  return (v + 100) ^ 1;
 fault:
  return (v + 100) ^ 2;
}

/* A + 100 is also computed on both of the second asm goto's paths, but that
   asm goto only writes B, so hoisting above it is correct and must still
   happen.  This is the only hoist the dump scan above expects.  */

__attribute__((noipa, cold)) long
other_operand (unsigned long *p)
{
  unsigned long a, b;

  get_user_asm (a, p, fault1);
  get_user_asm (b, p + 1, fault2);
  return ((a + 100) ^ 1) + b;
 fault2:
  return (a + 100) ^ 2;
 fault1:
  return -14;
}

int
main (void)
{
  unsigned long vals[2];

  vals[0] = 5;
  if (both_paths (vals) != ((5 + 100) ^ 1))
    __builtin_abort ();
  vals[0] = 0;
  if (both_paths (vals) != ((0 + 100) ^ 2))
    __builtin_abort ();

  vals[0] = 5;
  vals[1] = 7;
  if (other_operand (vals) != ((5 + 100) ^ 1) + 7)
    __builtin_abort ();
  vals[1] = 0;
  if (other_operand (vals) != ((5 + 100) ^ 2))
    __builtin_abort ();
  vals[0] = 0;
  if (other_operand (vals) != -14)
    __builtin_abort ();

  return 0;
}
