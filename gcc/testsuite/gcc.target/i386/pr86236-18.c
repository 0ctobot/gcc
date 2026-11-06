/* PR target/86236 */
/* { dg-do run { target lp64 } } */
/* { dg-options "-O2 -mgeneral-regs-only -mforce-drap -mstackrealign -fno-stack-protector -fcall-used-r13 -ffixed-rbx -ffixed-rax -ffixed-r9 -ffixed-r10 -ffixed-r11" } */

/* -fcall-used-r13 lets an indirect sibcall hold its target in R13 across the
   epilogue, so a function with a tail call can't reload the DRAP register
   into it.  */

typedef long (*fp5) (long, long, long, long, long);

__attribute__ ((noipa)) long
t5 (long a, long b, long c, long d, long e)
{
  return a + 2 * b + 3 * c + 4 * d + 5 * e;
}

__attribute__ ((noipa)) long
f (fp5 fn, long a, long b, long c, long d)
{
  volatile long v __attribute__ ((aligned (64))) = d;
  return fn (a, b, c, d, v);
}

int
main (void)
{
  volatile fp5 fn = t5;
  if (f (fn, 1, 2, 3, 4) != 50)
    __builtin_abort ();
  return 0;
}
