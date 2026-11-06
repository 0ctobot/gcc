/* PR middle-end/126468 */
/* { dg-do run } */
/* { dg-additional-options "-fno-tree-ccp -w" } */

/* Reading a constant out of a static initializer shouldn't carry TREE_OVERFLOW
   over to the call.  CCP would otherwise hide this by dropping the flag.  */

static const struct { long long a; long long b; } s
  = { __LONG_LONG_MAX__ + 1LL, 2 };

__attribute__((noipa)) int
ctor (void)
{
  return __builtin_clrsbll (s.a);
}

int
main (void)
{
  if (ctor () != 0)
    __builtin_abort ();
  return 0;
}
