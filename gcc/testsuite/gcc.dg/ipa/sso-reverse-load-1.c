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

struct __attribute__((scalar_storage_order (REVERSE))) word { u32 v; };

__attribute__((noinline)) static u32
read_word (const struct word *p)
{
  return p->v;
}

__attribute__((noinline)) static u32
copy_word (u32 n)
{
  struct word a[2];
  __builtin_memcpy ((unsigned char *) a + sizeof a[0], &n, sizeof n);
  return read_word (&a[1]);
}
#endif

int
main (void)
{
#if __CHAR_BIT__ == 8 && defined (__UINT32_TYPE__)
  if (copy_word (0x12345678U) != 0x78563412U)
    __builtin_abort ();
#endif
  return 0;
}
