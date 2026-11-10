/* PR rtl-optimization/128570 */
/* { dg-do run } */
/* { dg-options "-O2" } */

/* loop2_invariant must not hoist a non-volatile asm with a "memory" clobber
   out of a loop that stores to the memory the asm reads or writes.  */

long g, h[4];

#define RD(v) \
  __asm__ ("adrp x9, g\n\tldr %0, [x9, :lo12:g]" : "=r" (v) : : "x9", "memory")
#define RDP(v, p) \
  __asm__ ("ldr %0, [%1]" : "=r" (v) : "r" (p) : "memory")
#define WRP(v, p, x) \
  __asm__ ("str %2, [%1]\n\tmov %0, %2" : "=&r" (v) : "r" (p), "r" (x) : "memory")

/* The asm reads g and the loop increments g.  */
__attribute__((noipa)) long
rd_global (int n)
{
  long s = 0, v;
  for (int i = 0; i < n; i++)
    {
      RD (v);
      s += v;
      g++;
    }
  return s;
}

/* The asm reads *p and the loop increments *p.  */
__attribute__((noipa)) long
rd_ptr (long *p, int n)
{
  long s = 0, a;
  do
    {
      RDP (a, p);
      s += a;
      (*p)++;
    }
  while (--n);
  return s;
}

/* The asm stores x to *p and returns x, and the loop then overwrites *p.  */
__attribute__((noipa)) long
wr_ptr (long *p, long x, int n)
{
  long s = 0, t;
  for (int i = 0; i < n; i++)
    {
      WRP (t, p, x);
      s += t + *p;
      *p = i;
    }
  return s;
}

int
main (void)
{
  g = 0;
  if (rd_global (3) != 3)
    __builtin_abort ();
  h[3] = 1;
  if (rd_ptr (h + 3, 4) != 10)
    __builtin_abort ();
  h[2] = 0;
  if (wr_ptr (h + 2, 50, 3) != 300)
    __builtin_abort ();
  if (h[2] != 2)
    __builtin_abort ();
  return 0;
}
