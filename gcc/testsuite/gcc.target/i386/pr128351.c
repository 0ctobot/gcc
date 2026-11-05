/* PR tree-optimization/128351 */
/* { dg-do run { target avx512f } } */
/* { dg-options "-O3 -mavx512f -mprefer-vector-width=512 --param vect-partial-vector-usage=2 -fdump-tree-vect-details" } */

#include "avx512f-check.h"

/* Each loop below has an early exit that is always taken at index 700,
   but the count for the main exit is larger than the bound that the
   arrays give the whole loop.  The decrementing IV for the AVX-512 masks
   starts from that count, so it must not be truncated to a type sized
   only for the bound.  The do-while loop with N == 0 makes the count for
   the main exit wrap to 0.  */

typedef __UINT32_TYPE__ u32;
typedef __UINT64_TYPE__ u64;

u32 a[1024];
u64 b[1024];

__attribute__((noipa)) int
t32 (u32 n, u32 key)
{
  for (u32 i = 0; i < n; i++)
    if (a[i] == key)
      return 1;
  return -1;
}

__attribute__((noipa)) int
t64 (u64 n, u64 key)
{
  for (u64 i = 0; i < n; i++)
    if (b[i] == key)
      return 1;
  return -1;
}

__attribute__((noipa)) int
d32 (u32 n, u32 key)
{
  u32 x = 0;
  do
    {
      if (a[x] == key)
	return 1;
      x++;
    }
  while (--n);
  return -1;
}

static void
avx512f_test (void)
{
  for (int i = 0; i < 1024; i++)
    {
      a[i] = i;
      b[i] = i;
    }

  if (t32 (0x10005, 700) != 1)
    abort ();
  if (t32 (0xffffffff, 700) != 1)
    abort ();
  if (t64 (0x100000005ULL, 700) != 1)
    abort ();
  if (t64 (-1ULL, 700) != 1)
    abort ();
  if (d32 (0, 700) != 1)
    abort ();
  if (d32 (0x10005, 700) != 1)
    abort ();
}

/* { dg-final { scan-tree-dump "operating on partial vectors" "vect" } } */
