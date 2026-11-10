/* { dg-lto-do run } */
/* { dg-lto-options {{-O2 -flto -fno-strict-overflow}} } */
/* { dg-suppress-ld-options {-fno-strict-overflow} } */

/* Both files are compiled with -fno-strict-overflow, but the link command
   line doesn't repeat it.  When the inliner evaluates callee's x + 1 < 0
   test for caller's argument range [0, INT_MAX], it must let the addition
   wrap, or it judges the overflow path unreachable and deletes it from the
   inlined copy.  */

extern int callee (int);

int handled;

__attribute__ ((noinline)) void
overflow_handler (void)
{
  handled = 1;
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
