/* PR target/126556 */
/* { dg-do run } */

typedef unsigned char u8;
typedef __UINT32_TYPE__ u32;
typedef u8 v4u8 __attribute__((vector_size (4)));

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define TOP 3
#else
#define TOP 0
#endif

/* Combine only learns that V6 is zero after it has merged the comparison's
   user into a ZERO_EXTEND, which then has to fold around the known result.  The
   vector must escape, since GIMPLE folds the whole function otherwise.  */
#define LOAD					\
  union { u32 u; v4u8 v; } t = { x & 0xffff };	\
  v4u8 b = t.v;					\
  u8 v6 = b[TOP]

/* Clearing byte IDX of VAL must give the bytes at P.  */
__attribute__((noipa)) void
check (const void *p, u32 val, int idx)
{
  u8 want[4];

  __builtin_memcpy (want, &val, sizeof (want));
  want[idx] = 0;
  if (__builtin_memcmp (p, want, sizeof (want)))
    __builtin_abort ();
}

__attribute__((noipa)) void
f1 (u32 x)
{
  LOAD;
  b[(signed char) v6 > 0] = v6;
  check (&b, x & 0xffff, 0);
}

__attribute__((noipa)) void
f2 (u32 x)
{
  LOAD;
  b[(signed char) v6 <= 0] = v6;
  check (&b, x & 0xffff, 1);
}

__attribute__((noipa)) void
f3 (u32 x)
{
  LOAD;
  b[(short) (signed char) v6 > 0] = v6;
  check (&b, x & 0xffff, 0);
}

__attribute__((noipa)) void
f4 (u32 x, u8 *p)
{
  LOAD;
  p[(signed char) v6 > 0] = v6;
  check (&b, x & 0xffff, TOP);
}

int
main (void)
{
  u8 out[2] = { 0xaa, 0xbb };

  f1 (0xdead1234);
  f2 (0xdead1234);
  f3 (0xdead1234);
  f4 (0xdead1234, out);
  if (out[0] != 0 || out[1] != 0xbb)
    __builtin_abort ();
  return 0;
}
