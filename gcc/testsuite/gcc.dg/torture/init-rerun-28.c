/* { dg-do run } */
/* { dg-require-effective-target nonlocal_goto } */
/* { dg-options "-ftrivial-auto-var-init=zero" } */

/* A goto out of a nested function leaves the block of V without running its
   cleanup.  Entering the block again at a label afterwards starts a new
   lifetime of V, so the first run of its initializer has to see V
   automatically initialized again rather than the value of the old run.  */

struct S { int a[4]; int k; };

__attribute__ ((noipa)) long long
nonlocal_goto (void)
{
  __label__ out;
  volatile long long res = 0;
  volatile int runs = 0, done = 0;
  __attribute__ ((noipa)) void leave (void) { goto out; }
  {
    res = 4;
  again:;
    struct S v = { .k = 1, .a[0] = runs ? v.k : v.k + 100, .a[1] = 2 };
    res = res * 1000 + v.a[0];
    if (++runs < 2)
      goto again;
    if (!done++)
      leave ();
    return res;
  }
 out:
  runs = 0;
  goto again;
}

int
main (void)
{
  if (nonlocal_goto () != 4100001100001LL)
    __builtin_abort ();
  return 0;
}
