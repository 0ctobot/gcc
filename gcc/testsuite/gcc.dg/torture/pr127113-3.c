/* { dg-do run { target bitint } } */
/* { dg-options "-std=c23" } */

/* Like pr127113-2.c, with a constant of a _BitInt type that has fewer bits than
   the byte stored to the union.  */

union u0
{
  unsigned _BitInt(7) f0;
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
  if (a0.f2 == 0xff)
    return;
  __builtin_abort ();
}

int
main (void)
{
  union u0 u;

  u.f1 = 0;
  u.f0 = 1;
  f12 (u, g31);
  return 0;
}
