/* PR tree-optimization/128351 */
/* { dg-add-options vect_early_break } */
/* { dg-require-effective-target vect_early_break_hw } */
/* { dg-require-effective-target vect_int } */
/* { dg-additional-options "-O3" } */
/* { dg-require-effective-target aarch64_sve_hw { target aarch64*-*-* } } */
/* { dg-additional-options "-march=armv8.2-a+sve" { target aarch64*-*-* } } */

/* { dg-final { scan-tree-dump "LOOP VECTORIZED" "vect" { target aarch64*-*-* } } } */

#include "tree-vect.h"

/* Search loops over arrays of known size with a 64-bit main exit count above
   2^32, while the key ends each of them early.  The bound that the array size
   gives covers the early exit, not the main exit, so a fully masked loop can't
   compare against its limit in 32 bits.  */

typedef __UINT64_TYPE__ u64;
typedef __UINT32_TYPE__ u32;

#define NI __attribute__ ((noipa))
#define N 1024

u32 t32[N];
u64 t64[N];

NI int
x1 (u64 n, u32 key)
{
  u64 i = 0;
  do
    {
      if (t32[i] == key)
	return i;
      i++;
    }
  while (--n);
  return -1;
}

NI int
x2 (u64 n, u64 key)
{
  u32 i = 0;
  do
    {
      if (t64[i] == key)
	return i;
      i++;
    }
  while (--n);
  return -1;
}

NI int
x3 (u64 n, u32 key)
{
  for (u64 i = 0; i < n; i++)
    if (t32[i] == key)
      return i;
  return -1;
}

int
main (void)
{
  check_vect ();

  for (int at = 0; at < 1000; at += 97)
    {
      for (int i = 0; i < N; i++)
	{
	  t32[i] = i * 2654435761u | 1;
	  t64[i] = i * 0x9e3779b97f4a7c15ull | 1;
	}
      t32[at] = 0x1000;
      t64[at] = 0x1000;
      u64 n = (1ull << 32) + 5;
      if (x1 (n, 0x1000) != at)
	__builtin_abort ();
      if (x2 (n, 0x1000) != at)
	__builtin_abort ();
      if (x3 (n, 0x1000) != at)
	__builtin_abort ();
    }

  return 0;
}
