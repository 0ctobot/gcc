/* { dg-do run } */
/* { dg-skip-if "mixed endian" { ! { le || be } } } */
/* { dg-options "-O2 -fno-ipa-sra" } */

#if __CHAR_BIT__ == 8 && defined (__UINT32_TYPE__)
typedef __UINT32_TYPE__ u32;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
# define REVERSE "big-endian"
#else
# define REVERSE "little-endian"
#endif

struct __attribute__((scalar_storage_order (REVERSE))) three { u32 a, b, c; };

/* Looking through the memcpy turns the reverse-order load of dst->a into a
   native-order load of src[0], which must not take the constant that IPA-CP
   recorded for the caller's buf[0].  */

__attribute__((noinline)) static u32
read_a (struct three *dst, const u32 *src)
{
  __builtin_memcpy (dst, src, sizeof *dst);
  return dst->a;
}

__attribute__((noinline)) static u32
copy (u32 n)
{
  u32 buf[3];
  struct three t;

  buf[0] = n;
  buf[1] = 0;
  buf[2] = 0;
  return read_a (&t, buf);
}
#endif

int
main (void)
{
#if __CHAR_BIT__ == 8 && defined (__UINT32_TYPE__)
  if (copy (0x12345678U) != 0x78563412U)
    __builtin_abort ();
#endif
  return 0;
}
