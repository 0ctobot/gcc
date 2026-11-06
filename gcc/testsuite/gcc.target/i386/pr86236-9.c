/* PR target/86236 */
/* { dg-do run { target lp64 } } */
/* { dg-options "-O2" } */

/* A function without callee-saved registers can tail call a preserve_none
   function, which takes its second argument in R13, so the epilogue must not
   reload R13.  */

volatile long seen;

__attribute__ ((noipa)) void
g (long a1, long a2, long a3, long a4, long a5, long a6, long a7, long a8)
{
  seen = a8;
}

__attribute__ ((noipa, preserve_none)) void
h (long a, long b)
{
  seen += a * b;
}

__attribute__ ((noipa, no_callee_saved_registers)) void
f (long v)
{
  volatile long b __attribute__ ((aligned (64)));
  b = 1;
  g (1, 2, 3, 4, 5, 6, 7, v);
  h (v, 9);
}

int
main (void)
{
  f (2);
  if (seen != 20)
    __builtin_abort ();
  return 0;
}
