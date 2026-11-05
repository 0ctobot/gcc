/* PR tree-optimization/128351 */
/* { dg-add-options vect_early_break } */
/* { dg-require-effective-target vect_early_break_hw } */
/* { dg-require-effective-target vect_int } */
/* { dg-additional-options "-O3" } */
/* { dg-require-effective-target aarch64_sve_hw { target aarch64*-*-* } } */
/* { dg-additional-options "-march=armv8.2-a+sve" { target aarch64*-*-* } } */

/* { dg-final { scan-tree-dump "LOOP VECTORIZED" "vect" { target aarch64*-*-* } } } */

#include "tree-vect.h"

/* Search loops over arrays of known size whose 32-bit main exit count is 2^32,
   which wraps to 0, while the key ends each of them early.  The bound that the
   array size gives covers the early exit, not the main exit, so a fully masked
   loop still has to undo the wrap of its limit.  */

typedef __UINT64_TYPE__ u64;
typedef __UINT32_TYPE__ u32;
typedef __UINT16_TYPE__ u16;
typedef __UINT8_TYPE__ u8;

#define NI __attribute__ ((noipa))
#define N 1024

u32 t32[N];
u64 t64[N];
u16 t16[N];
u8 t8[N];

NI int
w1 (u32 n, u32 key)
{
  u32 i = 0;
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
w2 (u32 n, u64 key)
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
w3 (u32 n, u16 key)
{
  u32 i = 0;
  do
    {
      if (t16[i] == key)
	return i;
      i++;
    }
  while (i != n);
  return -1;
}

NI int
w4 (u32 n, u8 key)
{
  u32 i = 0;
  do
    {
      if (t8[i] == key)
	return i;
      i++;
    }
  while (i != n);
  return -1;
}

NI int
w5 (u32 s, u32 e, u32 key)
{
  int i = 0;
  do
    {
      if (t32[i] == key)
	return i;
      i++;
    }
  while (++s != e);
  return -1;
}

NI u64
w6 (u32 n, u64 key)
{
  u64 s = 0;
  u32 i = 0;
  do
    {
      if (t64[i] == key)
	break;
      s += t64[i];
      i++;
    }
  while (--n);
  return s;
}

NI int
w7 (u64 n, u32 key)
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
w8 (u16 n, u16 key)
{
  u32 i = 0;
  do
    {
      if (t16[i] == key)
	return i;
      i++;
    }
  while (--n);
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
	  t16[i] = i | 0x8000;
	  t8[i] = 1 + i % 200;
	}
      t32[at] = 0x1000;
      t64[at] = 0x1000;
      t16[at] = 0x1000;
      t8[at] = 0xee;
      u64 want6 = 0;
      for (int i = 0; i < at; i++)
	want6 += t64[i];

      if (w1 (0, 0x1000) != at)
	__builtin_abort ();
      if (w2 (0, 0x1000) != at)
	__builtin_abort ();
      if (w3 (0, 0x1000) != at)
	__builtin_abort ();
      if (w4 (0, 0xee) != at)
	__builtin_abort ();
      if (w5 (9, 9, 0x1000) != at)
	__builtin_abort ();
      if (w6 (0, 0x1000) != want6)
	__builtin_abort ();
      if (w7 (0, 0x1000) != at)
	__builtin_abort ();
      if (w8 (0, 0x1000) != at)
	__builtin_abort ();
    }

  return 0;
}
