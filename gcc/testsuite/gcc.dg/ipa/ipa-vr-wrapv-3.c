/* { dg-do run } */
/* { dg-options "-O2 -fno-early-inlining" } */

/* The whole file asks for wrapping signed overflow, but the command line
   doesn't.  When the inliner evaluates callee's x + 1 < 0 test for
   caller's argument range [0, INT_MAX], it must let the addition wrap, or
   it judges the overflow path unreachable and deletes it from the inlined
   copy.  */

#pragma GCC optimize ("wrapv")

int handled;

__attribute__ ((noinline)) void
overflow_handler (void)
{
  handled = 1;
}

int
callee (int x)
{
  /* True for x == INT_MAX, since the addition wraps.  */
  if (x + 1 < 0)
    {
      overflow_handler ();
      return -1;
    }
  return x + 1;
}

__attribute__ ((noinline)) int
caller (unsigned int v)
{
  return callee (v & 0x7fffffff);
}

int
main (void)
{
  volatile unsigned int v = 0x7fffffff;

  if (caller (v) != -1 || !handled)
    __builtin_abort ();
  return 0;
}
