/* { dg-do run { target bitint } } */
/* { dg-options "-std=c23" } */

/* Like pr127113-3.c, with a _BitInt constant too large for a machine mode.  */

#if __BITINT_MAXWIDTH__ >= 192
union u0
{
  unsigned _BitInt(191) f0;
  unsigned _BitInt(192) f2;
  unsigned int f1;
  unsigned char pad[128];
};

unsigned short g31;

__attribute__((noinline)) static void
f12 (union u0 a0, unsigned short a3)
{
  if (a3)
    goto out;
  a0.f1 = ~g31;
  a0.f2 = (unsigned _BitInt(192)) (int) a0.f1;
out:
  if (a0.f2 != (unsigned _BitInt(192)) -1)
    __builtin_abort ();
}
#endif

int
main (void)
{
#if __BITINT_MAXWIDTH__ >= 192
  union u0 l = { .f0 = 1 };
  f12 (l, g31);
#endif
  return 0;
}
