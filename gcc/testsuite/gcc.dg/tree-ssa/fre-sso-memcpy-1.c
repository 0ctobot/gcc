/* { dg-do run } */
/* { dg-skip-if "mixed endian" { ! { le || be } } } */
/* { dg-options "-O2" } */

#if __CHAR_BIT__ == 8 && defined (__UINT32_TYPE__)
typedef __UINT32_TYPE__ u32;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
# define REVERSE "big-endian"
#else
# define REVERSE "little-endian"
#endif

struct __attribute__((scalar_storage_order (REVERSE))) three { u32 a, b, c; };

/* The reverse-order load of dst->a reads the bytes of src[0], so it has to see
   them byte-swapped rather than the value stored to src[0].  */

__attribute__((noipa)) u32
copy_and_read (struct three *dst, u32 *src, u32 n)
{
  src[0] = n;
  __builtin_memcpy (dst, src, sizeof *dst);
  return dst->a;
}
#endif

int
main (void)
{
#if __CHAR_BIT__ == 8 && defined (__UINT32_TYPE__)
  u32 buf[3] = { 0, 0, 0 };
  struct three t;

  if (copy_and_read (&t, buf, 0x12345678U) != 0x78563412U)
    __builtin_abort ();
#endif
  return 0;
}
