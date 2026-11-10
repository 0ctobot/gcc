/* { dg-do run } */
/* { dg-additional-options "-ftrivial-auto-var-init=zero -Wno-uninitialized" } */
/* { dg-require-effective-target nonlocal_goto } */

/* A goto out of a nested function leaves the block of X without running its
   cleanup.  Entering the block again at a label above the declaration of X
   starts a new lifetime of X, so the first run of its initializer has to see
   X zeroed again rather than the value from the old lifetime.  A jump back
   within the same lifetime still sees the value of the previous run.  */

__attribute__ ((noipa)) int
nonlocal_goto (void)
{
  __label__ out;
  volatile int pass = 0;
  volatile int result = 0;
  __attribute__ ((noipa)) void leave (void) { goto out; }
  {
  again:;
    volatile int x = x + 1;
    result = result * 10 + x;
    if (pass == 0)
      {
	pass = 1;
	goto again;
      }
    if (pass == 1)
      {
	pass = 2;
	leave ();
      }
    return result;
  }
 out:
  goto again;
}

int
main (void)
{
  if (nonlocal_goto () != 121)
    __builtin_abort ();
  return 0;
}
