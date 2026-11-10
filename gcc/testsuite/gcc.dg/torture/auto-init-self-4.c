/* { dg-do run } */
/* { dg-additional-options "-ftrivial-auto-var-init=zero -Wno-uninitialized" } */

/* A nested function reaches a variable of its parent through the static
   chain, even before the declaration of the variable, so the variable's
   initializer can read it through a call to the nested function or through
   an address the nested function took.  The variable still gets zeroed
   before the initializer runs.  */

int *addr;

__attribute__((noipa)) void
dirty (void)
{
  volatile unsigned char buf[512];
  int i;

  for (i = 0; i < 512; i++)
    buf[i] = 0xa5;
}

__attribute__((noipa)) int
self_nested (void)
{
  auto int __attribute__((noipa)) get (void);
  int x = get ();
  int __attribute__((noipa)) get (void) { return x; }
  return x;
}

__attribute__((noipa)) int
self_nested_addr (void)
{
  auto void __attribute__((noipa)) take (void);
  take ();
  int x = *addr;
  void __attribute__((noipa)) take (void) { addr = &x; }
  return x;
}

int
main (void)
{
  dirty ();
  if (self_nested () != 0)
    __builtin_abort ();
  dirty ();
  if (self_nested_addr () != 0)
    __builtin_abort ();
  return 0;
}
