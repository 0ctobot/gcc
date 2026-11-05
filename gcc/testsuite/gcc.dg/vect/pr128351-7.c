/* PR tree-optimization/128351 */
/* { dg-add-options vect_early_break } */
/* { dg-require-effective-target vect_early_break_hw } */
/* { dg-require-effective-target vect_int } */
/* { dg-additional-options "-O3" } */
/* { dg-require-effective-target aarch64_sve_hw { target aarch64*-*-* } } */
/* { dg-additional-options "-march=armv8.2-a+sve" { target aarch64*-*-* } } */

/* { dg-final { scan-tree-dump-times "LOOP VECTORIZED" 4 "vect" { target aarch64*-*-* } } } */

#include "tree-vect.h"

/* Search loops over a possibly misaligned pointer, with a 64-bit main exit
   count near 2^64 and the key ending each search early.  A fully masked
   loop can mask off the leading elements to reach an aligned address, and
   adding their number to the count must not wrap the limit that the masks
   are built from.  */

typedef __UINT64_TYPE__ u64;

#define NI __attribute__ ((noipa))
#define N 1024

NI long
f1 (u64 *p, u64 n, u64 key)
{
  for (u64 i = 0; i < n; i++)
    if (p[i] == key)
      return i;
  return -1;
}

NI long
f2 (u64 *p, u64 n, u64 key)
{
  for (u64 i = 0; i != n; i++)
    if (p[i] == key)
      return i;
  return -1;
}

NI long
f3 (u64 *p, u64 n, u64 key)
{
  long x = 0;
  if (!n)
    return -1;
  do
    {
      if (p[x] == key)
	return x;
      x++;
    }
  while (--n);
  return -1;
}

NI long
f4 (u64 *p, u64 n, u64 key)
{
  long x = 0;
  while (n--)
    {
      if (p[x] == key)
	return x;
      x++;
    }
  return -1;
}

static u64 buf[N] __attribute__ ((aligned (4096)));

int
main (void)
{
  check_vect ();

  for (int off = 0; off < 32; off++)
    for (int k = 1; k <= 32; k++)
      {
	u64 n = -(u64) k;
	int at = 100;
#pragma GCC novector
	for (int i = 0; i < N; i++)
	  buf[i] = i * 0x9e3779b97f4a7c15ull | 1;
	buf[off + at] = 0x1000;
	if (f1 (buf + off, n, 0x1000) != at)
	  __builtin_abort ();
	if (f2 (buf + off, n, 0x1000) != at)
	  __builtin_abort ();
	if (f3 (buf + off, n, 0x1000) != at)
	  __builtin_abort ();
	if (f4 (buf + off, n, 0x1000) != at)
	  __builtin_abort ();
      }

  return 0;
}
