/* PR ipa/128469 */
/* { dg-do run } */
/* { dg-options "-O2" } */
/* { dg-require-effective-target nonlocal_goto } */

/* Stores done by a nested function before it jumps to a nonlocal label
   in its parent must stay visible to the parent's caller.  */

volatile int c = 1;

__attribute__((noinline)) void
f1 (int *p)
{
  __label__ out;
  void nf (void) { *p = 0; goto out; }
  nf ();
out:;
}

__attribute__((noinline)) void
f2 (int *p)
{
  __label__ out;
  void nf (void) { *p = 0; goto out; }
  void __attribute__((noinline)) mid (void) { nf (); }
  mid ();
out:;
}

__attribute__((noinline)) void
f3 (int *p)
{
  __label__ out;
  void __attribute__((noinline)) nf (void) { *p = 0; goto out; }
  void __attribute__((noinline)) mid (void) { if (c) nf (); }
  mid ();
out:;
}

__attribute__((noinline)) void
f4 (int *p)
{
  __label__ out;
  void __attribute__((noinline)) nf (int *q) { *q = 0; goto out; }
  nf (p);
out:;
}

int
main (void)
{
  int i;

  i = 1;
  f1 (&i);
  if (i != 0)
    __builtin_abort ();

  i = 1;
  f2 (&i);
  if (i != 0)
    __builtin_abort ();

  i = 1;
  f3 (&i);
  if (i != 0)
    __builtin_abort ();

  i = 1;
  f4 (&i);
  if (i != 0)
    __builtin_abort ();

  return 0;
}
