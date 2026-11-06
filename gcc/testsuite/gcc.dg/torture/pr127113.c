/* { dg-do run } */

/* IPA-CP knows the union holds a bool constant and IPA-SRA replaced the
   component with a local bool variable, which can't hold the byte stored to
   it.  */

union u0
{
  _Bool f0;
  unsigned int f1;
  unsigned char f2;
};

unsigned short g31;

static void
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
