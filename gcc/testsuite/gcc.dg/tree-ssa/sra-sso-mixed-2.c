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
struct __attribute__((scalar_storage_order (REVERSE))) word { u32 v; };
struct pair { struct word w[2]; };

/* The native store into A and the reverse-order read of B meet across the copy
   of A into B, so a scalar copy between their replacements would return the
   native value.  */

__attribute__((noipa))
u32
f (u32 n)
{
  struct word a[2], b[2];
  __builtin_memcpy ((unsigned char *) a + sizeof a[0], &n, sizeof n);
  b[1] = a[1];
  return b[1].v;
}

/* The same with the copy split into the fields of the aggregate.  */

__attribute__((noipa))
u32
g (u32 n)
{
  struct pair a, b;
  a.w[0].v = 0;
  __builtin_memcpy ((unsigned char *) &a + sizeof a.w[0], &n, sizeof n);
  b = a;
  return b.w[1].v;
}
#endif

int
main (void)
{
#if __CHAR_BIT__ == 8 && defined (__UINT32_TYPE__)
  if (f (0x12345678U) != 0x78563412U)
    __builtin_abort ();
  if (g (0x12345678U) != 0x78563412U)
    __builtin_abort ();
#endif
  return 0;
}
