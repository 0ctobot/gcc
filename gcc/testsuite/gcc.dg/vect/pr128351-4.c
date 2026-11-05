/* PR tree-optimization/128351 */
/* { dg-add-options vect_early_break } */
/* { dg-require-effective-target vect_early_break_hw } */
/* { dg-require-effective-target vect_int } */
/* { dg-additional-options "-O3" } */
/* { dg-require-effective-target aarch64_sve_hw { target aarch64*-*-* } } */
/* { dg-additional-options "-march=armv8.2-a+sve" { target aarch64*-*-* } } */

/* { dg-final { scan-tree-dump "LOOP VECTORIZED" "vect" { target aarch64*-*-* } } } */

#include "tree-vect.h"

/* Search loops whose 64-bit main exit count is 2^64, which wraps to 0, while
   the key ends each of them early.  A fully masked loop must not take the
   wrapped count as its limit.  */

typedef __UINT64_TYPE__ u64;
typedef __UINT32_TYPE__ u32;
typedef __UINT8_TYPE__ u8;

#define NI __attribute__ ((noipa))

NI long
v0 (u64 *p, u64 n, u64 key)
{
  long x = 0;
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
v1 (u64 *p, u64 s, u64 e, u64 key)
{
  long x = 0;
  do
    {
      if (p[x] == key)
	return x;
      x++;
    }
  while (++s != e);
  return -1;
}

NI u8 *
v2 (u8 *p, u8 *end, u8 key)
{
  do
    {
      if (*p == key)
	return p;
      p++;
    }
  while (p != end);
  return 0;
}

NI long
v3 (u32 *p, u64 n, u32 key)
{
  long x = 0;
  do
    {
      if (p[x] == key)
	return x;
      x++;
    }
  while (--n);
  return -1;
}

NI u64
v4 (u64 *p, u64 n, u64 key)
{
  u64 s = 0;
  long x = 0;
  do
    {
      if (p[x] == key)
	break;
      s += p[x];
      x++;
    }
  while (--n);
  return s;
}

NI long
v5 (u64 *p, u64 n, u64 key)
{
  long x = 0;
  do
    {
      if (p[x] > key)
	return x;
      x++;
    }
  while (--n != 0);
  return -1;
}

NI long
v6 (u64 *p, u64 n, u64 key)
{
  long x = 0;
  for (;;)
    {
      if (p[x] == key)
	return x;
      x++;
      if (--n == 0)
	return -1;
    }
}

NI long
v7 (u64 *p, u64 n, u64 key)
{
  u64 i = 0;
  do
    {
      if (p[i] == key)
	return i;
      i++;
    }
  while (i != n);
  return -1;
}

#define N 512
/* Each element of BUF has its top bit clear, so that only the key is greater
   than KEY - 1.  */
#define KEY 0xfedcba9876543210ull

static u64 buf[N] __attribute__ ((aligned (256)));

int
main (void)
{
  check_vect ();

  for (int off = 0; off < 3; off++)
    for (int at = 0; at < 300; at += 53)
      {
	for (int i = 0; i < N; i++)
	  buf[i] = (i * 0x9e3779b97f4a7c15ull + 1) >> 1;
	buf[off + at] = KEY;
	u64 want4 = 0;
	for (int i = 0; i < at; i++)
	  want4 += buf[off + i];

	if (v0 (buf + off, 0, KEY) != at)
	  __builtin_abort ();
	if (v1 (buf + off, 77, 77, KEY) != at)
	  __builtin_abort ();
	if (v4 (buf + off, 0, KEY) != want4)
	  __builtin_abort ();
	if (v5 (buf + off, 0, KEY - 1) != at)
	  __builtin_abort ();
	if (v6 (buf + off, 0, KEY) != at)
	  __builtin_abort ();
	if (v7 (buf + off, 0, KEY) != at)
	  __builtin_abort ();

	u32 *b32 = (u32 *) buf;
	b32[2 * (off + at)] = 0xabcdef01u;
	b32[2 * (off + at) + 1] = 0;
	if (v3 (b32 + off, 0, 0xabcdef01u) != 2 * at + off)
	  __builtin_abort ();

	u8 *b8 = (u8 *) buf;
	for (int i = 0; i < N * 8; i++)
	  b8[i] = 1;
	b8[off + at] = 0xee;
	if (v2 (b8 + off, b8 + off, 0xee) != b8 + off + at)
	  __builtin_abort ();
      }

  return 0;
}
