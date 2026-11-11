/* Padding bits of a _BitInt can only hold arbitrary values on targets whose
   ABI leaves them undefined.  */
/* { dg-do run { target { bitint && { aarch64*-*-* i?86-*-* x86_64-*-* } } } } */
/* { dg-options "-std=gnu23" } */
/* { dg-additional-options "-mstrict-align" { target aarch64*-*-* } } */

/* A _BitInt member of a packed structure is misaligned, so a strict alignment
   target reads it like a bit-field of its whole storage size.  Reading it has
   to ignore the padding bits above its precision all the same.  */

#if __BITINT_MAXWIDTH__ >= 40
struct S
{
  unsigned char c;
  unsigned _BitInt(24) u;
  _BitInt(40) s;
} __attribute__((packed));

union U
{
  unsigned char b[sizeof (struct S)];
  struct S s;
};

[[gnu::noipa]] unsigned long long
get_u (struct S *p)
{
  return p->u;
}

[[gnu::noipa]] long long
get_s (struct S *p)
{
  return p->s;
}
#endif

int
main ()
{
#if __BITINT_MAXWIDTH__ >= 40
  /* Each member is stored in little-endian order, followed by padding.  */
  static const unsigned char set_padding[] = {
    0x00, 0x56, 0x34, 0x12, 0xff, 0x9a, 0x78, 0x56, 0x34, 0x12, 0xff, 0xff,
    0xff
  };
  static const unsigned char clear_padding[] = {
    0x00, 0x56, 0x34, 0x12, 0x00, 0x9a, 0x78, 0x56, 0x34, 0x92, 0x00, 0x00,
    0x00
  };
  union U x;

  if (sizeof (struct S) != sizeof (set_padding)
      || __builtin_offsetof (struct S, s) != 5)
    return 0;

  __builtin_memcpy (x.b, set_padding, sizeof (x.b));
  if (get_u (&x.s) != 0x123456)
    __builtin_abort ();
  if (get_s (&x.s) != 0x123456789a)
    __builtin_abort ();

  /* The sign bit of the 40-bit value is set, while the padding is clear.  */
  __builtin_memcpy (x.b, clear_padding, sizeof (x.b));
  if (get_s (&x.s) != 0x923456789a - 0x10000000000)
    __builtin_abort ();
#endif
}
