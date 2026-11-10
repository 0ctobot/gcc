/* { dg-do run } */
/* { dg-options "-O2" } */

/* Only f asks for wrapping signed overflow.  IPA-CP must let f's x + 1 wrap
   when it computes the range of g's parameter, or it drops INT_MIN from
   that range and folds g's y > 0 test to true.  */

int sink;

static __attribute__ ((noinline)) int
g (int y)
{
  sink++;
  return y > 0 ? 1 : 2;
}

static __attribute__ ((noinline, optimize ("wrapv"))) int
f (int x)
{
  sink += 2;
  return g (x + 1);
}

__attribute__ ((noipa)) int h1 (void) { return f (5); }
__attribute__ ((noipa)) int h2 (void) { return f (__INT_MAX__); }
__attribute__ ((noipa)) int h3 (void) { return f (77); }

int
main (void)
{
  if (h1 () != 1 || h2 () != 2 || h3 () != 1)
    __builtin_abort ();
  return 0;
}
