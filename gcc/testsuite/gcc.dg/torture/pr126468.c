/* PR middle-end/126468 */
/* { dg-do run } */
/* { dg-require-effective-target int32 } */
/* { dg-additional-options "-fwrapv" } */

/* Each function computes a wrapped constant at compile time and passes it to
   __builtin_clrsb, whose expansion crashed on a constant with TREE_OVERFLOW
   set.  The wrapping is well defined under -fwrapv.  */

/* Final value replacement in sccp.  */

__attribute__((noipa)) int
scev_ll (void)
{
  long long x = 0;
  do
    x += 60976676962555186LL;
  while (x > 0);
  return __builtin_clrsbll (x);
}

__attribute__((noipa)) int
scev_int (void)
{
  int x = 0;
  for (int i = 0; i < 3; i++)
    x += 1000000000;
  return __builtin_clrsb (x);
}

/* Constant merging in reassoc.  */

__attribute__((noipa)) int
reassoc (long long a, long long b)
{
  return __builtin_clrsbll (a + 0x7000000000000000LL + b + 0x7000000000000000LL
			    - a - b);
}

/* Pass-through jump function arithmetic in IPA-CP.  */

__attribute__((noinline)) static int
ipa_leaf (long long x, long long k)
{
  long long r = 0;
  for (long long i = 0; i < k; i++)
    r += __builtin_clrsbll (x + i);
  return r + __builtin_clrsbll (x);
}

__attribute__((noinline)) static int
ipa_mid (long long y, long long k)
{
  return (ipa_leaf (y + 0x7000000000000000LL, k + 3)
	  + ipa_leaf (y + 0x7000000000000000LL, k * 3));
}

__attribute__((noipa)) int
ipa (long long k)
{
  return (ipa_mid (0x7000000000000000LL, k)
	  + ipa_mid (0x7000000000000000LL, k * 7));
}

int
main (void)
{
  if (scev_ll () != 0
      || scev_int () != 0
      || reassoc (1, 2) != 2
      || ipa (1) != 84)
    __builtin_abort ();
  return 0;
}
