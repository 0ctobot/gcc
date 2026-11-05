/* AArch64 version of gcc.target/i386/asm-goto-hoist-2.c: at -Os RTL code
   hoisting used to move the read of the flags output in front of the
   asm goto that sets it.  */
/* { dg-do run } */
/* { dg-options "-Os" } */

int v;

[[gnu::noipa]] void
foo (void)
{
  v = 42;
}

[[gnu::noipa]] void
bar (void)
{
  v = 43;
}

[[gnu::noipa]] int
baz (void)
{
  bool err;
  __asm goto ("ldr\tw1, %1\n\t"
	      "cmp\tw1, 0\n\t"
	      "b.ge\t%l[lab]\n\t"
	      "cmn\tw1, 1"
	      : "=@cceq" (err) : "m" (v) : "x1" : lab);
  if (err)
    {
      bar ();
      return -1;
    }
  return 0;

 lab:
  if (err)
    {
      foo ();
      return -2;
    }

  return 1;
}

int
main ()
{
  v = 0;
  if (baz () != -2 || v != 42)
    __builtin_abort ();
  v = 1;
  if (baz () != 1 || v != 1)
    __builtin_abort ();
  v = -1;
  if (baz () != -1 || v != 43)
    __builtin_abort ();
  v = -2;
  if (baz () != 0 || v != -2)
    __builtin_abort ();
}
