/* PR target/86236 */
/* { dg-do run { target lp64 } } */
/* { dg-options "-O2" } */

/* A varargs callee takes AX and a static chain takes R10, which leaves R11 for
   the indirect sibcall target, and the epilogue must not reload it.  */

typedef void pf (long, long, long, long, long, long, ...);

volatile long seen;

__attribute__ ((noipa)) void
g (long a1, long a2, long a3, long a4, long a5, long a6, long a7, long a8)
{
  seen = a8;
}

__attribute__ ((noipa)) void
t (long a1, long a2, long a3, long a4, long a5, long a6, ...)
{
  seen += a1 + a6;
}

__attribute__ ((noipa, preserve_none)) void
f (pf *p, long c)
{
  volatile long b __attribute__ ((aligned (64)));
  b = 1;
  g (1, 2, 3, 4, 5, 6, 7, c);
  __builtin_call_with_static_chain (p (1, 2, 3, 4, 5, 6), (void *) c);
}

int
main (void)
{
  f (t, 10);
  if (seen != 17)
    __builtin_abort ();
  return 0;
}
