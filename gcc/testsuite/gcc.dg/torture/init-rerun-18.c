/* { dg-do run } */
/* { dg-options "-ftrivial-auto-var-init=zero" } */
/* { dg-require-effective-target indirect_jumps } */
/* { dg-require-effective-target label_values } */

/* A computed goto out of a block skips the cleanup that clears the flag of a
   variable whose initializer gets built in a temporary.  An entry to the block
   after that, through a goto from outside of the block or a computed goto,
   still has to start over from the zeroes of automatic variable initialization.
   An asm goto out of the block runs the cleanup.  A computed goto inside of the
   block runs the initializer again, which has to read the value of the first
   run.  */

struct S { int a[4]; int k; };

__attribute__ ((noipa)) void
clobber (void)
{
  volatile char b[256];
  for (int i = 0; i < 256; i++)
    b[i] = 0x5a;
}

/* The re-run goes through a computed goto too.  */
__attribute__ ((noipa)) long long
computed_goto (int n)
{
  static void *const where[] = { &&again, &&out };
  long long res = 0;
  int runs = 0, done = 0;
  {
    res = 3;
  again:;
    struct S v = { .k = 1, .a[0] = runs ? v.k : v.k + 100, .a[1] = 2 };
    res = res * 1000 + v.a[0];
    if (++runs < 2)
      goto *where[0];
    goto *where[n];
  }
out:
  if (!done++)
    {
      runs = 0;
      goto *where[0];
    }
  return res;
}

/* The label that the computed goto out of the block jumps to comes ahead of
   the block, like the top of an interpreter loop.  */
__attribute__ ((noipa)) long long
dispatch (int n)
{
  static void *const ops[] = { &&top, &&again };
  long long res = 0;
  int runs = 0, pass = 0;
top:
  if (pass++)
    {
      runs = 0;
      goto again;
    }
  {
    res = 9;
  again:;
    struct S v = { .k = 1, .a[0] = runs ? v.k : v.k + 100, .a[1] = 2 };
    res = res * 1000 + v.a[0];
    if (++runs < 2)
      goto *ops[n];
    if (pass < 2)
      goto *ops[0];
  }
  return res;
}

#if defined (__x86_64__) || defined (__i386__)
#define JUMP "jmp %l0"
#elif defined (__aarch64__)
#define JUMP "b %l0"
#endif

#ifdef JUMP
__attribute__ ((noipa)) long long
asm_goto (void)
{
  long long res = 0;
  int runs = 0, done = 0;
  {
    res = 6;
  again:;
    struct S v = { .k = 1, .a[0] = runs ? v.k : v.k + 100, .a[1] = 2 };
    res = res * 1000 + v.a[0];
    if (++runs < 2)
      goto again;
    if (!done++)
      asm goto (JUMP :::: out);
  }
  return res;
out:
  runs = 0;
  goto again;
}
#endif

int
main (void)
{
  clobber ();
  if (computed_goto (1) != 3100001100001LL)
    __builtin_abort ();
  clobber ();
  if (dispatch (1) != 9100001100001LL)
    __builtin_abort ();
#ifdef JUMP
  clobber ();
  if (asm_goto () != 6100001100001LL)
    __builtin_abort ();
#endif
  return 0;
}
