/* { dg-do run } */
/* { dg-skip-if "mixed endian" { ! { le || be } } } */
/* { dg-options "-O2" } */

/* IPA-CP used to record a value stored into a structure with reverse scalar
   storage order as if it had been stored in native order.  A callee that
   copied the bytes of the member into a native integer then got the value
   back with its bytes in native order instead of swapped.  One of the two
   structures below has reverse storage order on any target.  */

typedef __UINT32_TYPE__ u32;

struct __attribute__((scalar_storage_order("big-endian"))) be
{
  u32 v;
};

struct __attribute__((scalar_storage_order("little-endian"))) le
{
  u32 v;
};

static const unsigned char be_bytes[4] = { 0x12, 0x34, 0x56, 0x78 };
static const unsigned char le_bytes[4] = { 0x78, 0x56, 0x34, 0x12 };

__attribute__((noinline)) static u32
read_be (const struct be *p)
{
  u32 r;
  __builtin_memcpy (&r, (const void *) p, sizeof r);
  return r;
}

__attribute__((noinline)) static u32
read_le (const struct le *p)
{
  u32 r;
  __builtin_memcpy (&r, (const void *) p, sizeof r);
  return r;
}

/* The stored value is a constant.  */

__attribute__((noinline)) static u32
const_be (void)
{
  struct be s;
  s.v = 0x12345678;
  return read_be (&s);
}

__attribute__((noinline)) static u32
const_le (void)
{
  struct le s;
  s.v = 0x12345678;
  return read_le (&s);
}

/* The stored value is the parameter of the caller.  */

__attribute__((noinline)) static u32
pass_be (u32 x)
{
  struct be s;
  s.v = x;
  return read_be (&s);
}

__attribute__((noinline)) static u32
pass_le (u32 x)
{
  struct le s;
  s.v = x;
  return read_le (&s);
}

int
main (void)
{
  u32 be_expected, le_expected;

  __builtin_memcpy (&be_expected, be_bytes, sizeof be_expected);
  __builtin_memcpy (&le_expected, le_bytes, sizeof le_expected);

  if (const_be () != be_expected)
    __builtin_abort ();
  if (const_le () != le_expected)
    __builtin_abort ();
  if (pass_be (0x12345678) != be_expected)
    __builtin_abort ();
  if (pass_le (0x12345678) != le_expected)
    __builtin_abort ();
  return 0;
}
