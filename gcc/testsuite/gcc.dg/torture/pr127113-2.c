/* { dg-do run } */

/* Like pr127113.c, but with a callee which can't be inlined before IPA-SRA
   runs, so that optimization levels with more aggressive early inlining also
   see the split.  */

union u0
{
  _Bool f0;
  unsigned int f1;
  unsigned char f2;
};

unsigned short g31;

__attribute__((noinline)) static void
f12 (union u0 a0, unsigned short a3)
{
  if (a3)
    goto lbl_br19;
  a0.f1 = ~g31;
  a0.f2 = a0.f1;
lbl_br19:
  if ((signed char) a0.f2 == -1LL)
    return;
  __builtin_abort ();
}

int
main (void)
{
  f12 ((union u0) { 1 }, g31);
  return 0;
}
