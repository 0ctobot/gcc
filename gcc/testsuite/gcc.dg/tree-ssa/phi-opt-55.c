/* { dg-do run } */
/* { dg-require-effective-target int32plus } */
/* { dg-options "-O2" } */

/* Conditional clears and copies of bits that must keep their meaning when
   the bit test looks at the value through a conversion, or when they don't
   fit the unconditional forms.  */

typedef __UINT64_TYPE__ u64;
typedef __UINT32_TYPE__ u32;

#define NI __attribute__ ((noipa))

/* The tested bit is the sign bit of the narrower type.  */
NI u64
clear_sign (u64 x)
{
  if ((int) x & (int) 0x80000000U)
    x &= ~0x80000000ULL;
  return x;
}

/* The cleared mask is the sign-extended test mask, which has bits that the
   test doesn't look at.  */
NI u64
clear_sext (u64 x)
{
  if ((int) x & (int) 0x80000000U)
    x &= ~0xffffffff80000000ULL;
  return x;
}

/* With a second bit in the mask the test stays a bit test, so these two
   reach the comparison of the cleared mask against the test mask.  */
NI u64
clear_sign_and_bit (u64 x)
{
  if ((int) x & (int) 0x80000004U)
    x &= ~0x80000004ULL;
  return x;
}

NI u64
clear_sext_and_bit (u64 x)
{
  if ((int) x & (int) 0x80000004U)
    x &= ~0xffffffff80000004ULL;
  return x;
}

NI u64
copy_sign_to_wider (int x, u64 y)
{
  if (x & (int) 0x80000000U)
    y |= 0x80000000ULL;
  return y;
}

NI u64
copy_sign_from_narrower (u64 x, u64 y)
{
  if ((int) x & (int) 0x80000000U)
    y |= 0x80000000ULL;
  return y;
}

/* With more than one bit, any of them sets the whole mask.  */
NI u64
copy_mask (u64 x, u64 y)
{
  if (x & 12)
    y |= 12;
  return y;
}

/* The copied bit isn't the tested one.  */
NI u64
copy_other (u64 x, u64 y)
{
  if (x & 4)
    y |= 8;
  return y;
}

NI u32
copy_to_narrower (u64 x, u32 y)
{
  if (x & (1ULL << 31))
    y |= 1U << 31;
  return y;
}

int
main (void)
{
  if (clear_sign (0x180000001ULL) != 0x100000001ULL)
    __builtin_abort ();
  if (clear_sign (0x100000001ULL) != 0x100000001ULL)
    __builtin_abort ();
  if (clear_sext (0x100000001ULL) != 0x100000001ULL)
    __builtin_abort ();
  if (clear_sext (0x180000001ULL) != 1)
    __builtin_abort ();
  if (clear_sign_and_bit (0x180000005ULL) != 0x100000001ULL)
    __builtin_abort ();
  if (clear_sign_and_bit (0x100000001ULL) != 0x100000001ULL)
    __builtin_abort ();
  if (clear_sext_and_bit (0x100000001ULL) != 0x100000001ULL)
    __builtin_abort ();
  if (clear_sext_and_bit (0x180000005ULL) != 1)
    __builtin_abort ();
  if (copy_sign_to_wider (-1, 0) != 0x80000000ULL)
    __builtin_abort ();
  if (copy_sign_to_wider (0x7fffffff, 1) != 1)
    __builtin_abort ();
  if (copy_sign_from_narrower (0xffffffffULL, 0) != 0x80000000ULL)
    __builtin_abort ();
  if (copy_sign_from_narrower (0x17fffffffULL, 0) != 0)
    __builtin_abort ();
  if (copy_mask (4, 1) != 13)
    __builtin_abort ();
  if (copy_mask (8, 0) != 12)
    __builtin_abort ();
  if (copy_mask (3, 0) != 0)
    __builtin_abort ();
  if (copy_other (4, 0) != 8)
    __builtin_abort ();
  if (copy_other (8, 0) != 0)
    __builtin_abort ();
  if (copy_to_narrower (0x80000000ULL, 1) != 0x80000001U)
    __builtin_abort ();
  if (copy_to_narrower (0x100000000ULL, 1) != 1)
    __builtin_abort ();
  return 0;
}
