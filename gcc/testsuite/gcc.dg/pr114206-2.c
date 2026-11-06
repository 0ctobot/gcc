/* PR tree-optimization/114206 */
/* A value smaller than a pointer can still carry a piece of an address, which
   lets a tail call compare it against the address in a later frame.  */
/* { dg-do run } */
/* { dg-require-effective-target lp64 } */
/* { dg-options "-O2" } */

__attribute__ ((noinline)) int
f (int n, unsigned lo, unsigned hi)
{
  int local = n;
  unsigned long me = (unsigned long) &local;
  if (lo == (unsigned) me && hi == (unsigned) (me >> 32))
    return -1;
  if (n == 0)
    return 0;
  return f (n - 1, (unsigned) me, (unsigned) (me >> 32));
}

__attribute__ ((noinline, pure)) int
p (int n, unsigned lo, unsigned hi)
{
  int local = n;
  unsigned long me = (unsigned long) &local;
  if (lo == (unsigned) me && hi == (unsigned) (me >> 32))
    return -1;
  if (n == 0)
    return 0;
  return p (n - 1, (unsigned) me, (unsigned) (me >> 32));
}

int
main (void)
{
  if (f (3, 0, 0))
    __builtin_abort ();
  if (p (3, 0, 0))
    __builtin_abort ();
  return 0;
}
