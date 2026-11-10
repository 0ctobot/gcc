/* { dg-do run } */
/* { dg-options "-O2 -mgeneral-regs-only -save-temps" } */

/* Check the values of the general-register popcount and parity
   expansions against a bit-by-bit count.  */

#pragma GCC target "+nocssc"

#define NI __attribute__ ((noipa))

NI static int
ref (unsigned long long x)
{
  int n = 0;
  for (int i = 0; i < 64; i++)
    n += (x >> i) & 1;
  return n;
}

NI static int pc32 (unsigned int x) { return __builtin_popcount (x); }
NI static int pc64 (unsigned long long x) { return __builtin_popcountll (x); }
NI static int par32 (unsigned int x) { return __builtin_parity (x); }
NI static int par64 (unsigned long long x) { return __builtin_parityll (x); }
NI static int pc128 (unsigned __int128 x) { return __builtin_popcountg (x); }
NI static int par128 (unsigned __int128 x) { return __builtin_parityg (x); }

static void
check (unsigned long long x, unsigned long long y)
{
  unsigned __int128 w = ((unsigned __int128) y << 64) | x;
  int n32 = ref ((unsigned int) x);
  int n64 = ref (x);
  int n128 = n64 + ref (y);

  if (pc32 (x) != n32 || par32 (x) != (n32 & 1)
      || pc64 (x) != n64 || par64 (x) != (n64 & 1)
      || pc128 (w) != n128 || par128 (w) != (n128 & 1))
    __builtin_abort ();
}

int
main (void)
{
  static const unsigned long long fixed[] = {
    0, 1, 0x80000000ULL, 0xffffffffULL, 0x100000000ULL,
    0x8000000000000000ULL, 0xffffffffffffffffULL, 0x5555555555555555ULL,
    0xaaaaaaaaaaaaaaaaULL, 0x0f0f0f0f0f0f0f0fULL, 0xf0f0f0f0f0f0f0f0ULL,
    0x0123456789abcdefULL
  };
  unsigned long long s = 0x9e3779b97f4a7c15ULL;

  for (unsigned int i = 0; i < sizeof fixed / sizeof fixed[0]; i++)
    for (unsigned int j = 0; j < sizeof fixed / sizeof fixed[0]; j++)
      check (fixed[i], fixed[j]);

  for (int i = 0; i < 64; i++)
    {
      check (1ULL << i, ~(1ULL << i));
      check ((1ULL << i) - 1, -(1ULL << i));
    }

  for (int i = 0; i < 10000; i++)
    {
      unsigned long long a, b;
      s = s * 6364136223846793005ULL + 1442695040888963407ULL;
      a = s;
      s = s * 6364136223846793005ULL + 1442695040888963407ULL;
      b = s;
      /* Vary the density of the set bits too.  */
      if (i & 1)
	a &= b;
      if (i & 2)
	b |= a;
      check (a, b);
    }

  return 0;
}

/* { dg-final { scan-assembler-not {__popcount} } } */
/* { dg-final { scan-assembler-not {__parity} } } */
