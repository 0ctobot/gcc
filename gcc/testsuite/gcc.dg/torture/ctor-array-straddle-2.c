/* The reads of ctor-array-straddle-1.c from static arrays that aren't
   declared const but are never written, which makes them read-only.  The
   addresses of the arrays must not escape, so this compares with the
   values the arrays hold instead of with a hidden read.  */
/* { dg-do run } */
/* { dg-require-effective-target int128 } */
/* { dg-require-effective-target le } */

typedef unsigned long long u64;
typedef unsigned int u32;
typedef unsigned short u16;

static __int128 ga[2] = { [1] = 0x80000000ULL };
static u32 t32[4] = { [1] = 0x11223344, [3] = 5 };
struct hdr { u16 tag; u32 f[3]; };
static struct hdr H = { .tag = 7, .f[1] = 0xaabbccdd };
struct P { u32 a; u16 b; };
static struct P arr[4] = { [2] = { 1, 2 }, [3] = { .a = 0x11223344 } };

__attribute__((noipa)) u64
rd_ga (void)
{
  u64 v;
  __builtin_memcpy (&v, (const char *) ga + 15, 8);
  return v;
}

__attribute__((noipa)) u16
rd_t32 (void)
{
  u16 v;
  __builtin_memcpy (&v, (const char *) t32 + 3, 2);
  return v;
}

__attribute__((noipa)) u32
rd_H (void)
{
  u32 v;
  __builtin_memcpy (&v, (const char *) &H + 6, 4);
  return v;
}

__attribute__((noipa)) u32
rd_arr (void)
{
  u32 v;
  __builtin_memcpy (&v, (const char *) arr + 22, 4);
  return v;
}

int
main (void)
{
  if (rd_ga () != 0x0000008000000000ULL)
    __builtin_abort ();
  if (rd_t32 () != 0x4400)
    __builtin_abort ();
  if (rd_H () != 0xccdd0000)
    __builtin_abort ();
  if (rd_arr () != 0x33440000)
    __builtin_abort ();
  return 0;
}
