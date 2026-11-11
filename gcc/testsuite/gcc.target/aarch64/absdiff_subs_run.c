/* { dg-do run } */
/* { dg-skip-if "" { *-*-* } { "-mcpu=*" "-march=*" "-mtune=*" } { "" } } */
/* { dg-options "-O3 -mcpu=neoverse-n2" } */

/* Check the absolute differences around the values where the compare
   flips.  */

typedef unsigned long long u64;
typedef long long s64;

__attribute__ ((noipa)) u64
ad64 (u64 a, u64 b)
{
  return a > b ? a - b : b - a;
}

__attribute__ ((noipa)) u64
ad64_ge (u64 a, u64 b)
{
  return a >= b ? a - b : b - a;
}

__attribute__ ((noipa)) u64
ads64 (s64 a, s64 b)
{
  return a > b ? (u64) a - (u64) b : (u64) b - (u64) a;
}

static const u64 vals[] = {
  0, 1, 2, 0x7ffffffffffffffeULL, 0x7fffffffffffffffULL,
  0x8000000000000000ULL, 0x8000000000000001ULL, 0xfffffffffffffffeULL,
  0xffffffffffffffffULL
};

int
main (void)
{
  const int n = sizeof (vals) / sizeof (vals[0]);
  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++)
      {
	u64 a = vals[i], b = vals[j];
	u64 d = a > b ? a - b : b - a;
	u64 sd = (s64) a > (s64) b ? a - b : b - a;
	if (ad64 (a, b) != d || ad64_ge (a, b) != d
	    || ads64 ((s64) a, (s64) b) != sd)
	  __builtin_abort ();
      }
  return 0;
}
