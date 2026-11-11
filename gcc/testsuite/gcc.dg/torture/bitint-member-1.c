/* Padding bits of a _BitInt can only hold arbitrary values on targets whose
   ABI leaves them undefined.  */
/* { dg-do run { target { bitint && { aarch64*-*-* i?86-*-* x86_64-*-* } } } } */
/* { dg-options "-std=gnu23" } */

/* A _BitInt member of a structure that is passed or returned in a register
   is read like a bit-field of its whole storage size.  Reading it has to
   ignore the padding bits above its precision all the same.  */

#if __BITINT_MAXWIDTH__ >= 40
struct T
{
  unsigned _BitInt(24) u;
  unsigned char c;
};

struct V
{
  _BitInt(40) s;
};

[[gnu::noipa]] unsigned long long
get_u (struct T t)
{
  return t.u;
}

[[gnu::noipa]] long long
get_s (struct V v)
{
  return v.s;
}

[[gnu::noipa]] struct T
copy_t (struct T *p)
{
  return *p;
}

[[gnu::noipa]] unsigned long long
get_returned_u (struct T *p)
{
  return copy_t (p).u;
}
#endif

int
main ()
{
#if __BITINT_MAXWIDTH__ >= 40
  /* Each member is stored in little-endian order, followed by padding.  */
  static const unsigned char t_bytes[] = {
    0x56, 0x34, 0x12, 0xff, 0x77, 0xff, 0xff, 0xff
  };
  /* The sign bit of the 40-bit value is set, while the padding is clear.  */
  static const unsigned char v_bytes[] = {
    0x9a, 0x78, 0x56, 0x34, 0x92, 0x00, 0x00, 0x00
  };
  struct T t;
  struct V v;

  if (sizeof (struct T) != sizeof (t_bytes)
      || sizeof (struct V) != sizeof (v_bytes))
    return 0;

  __builtin_memcpy (&t, t_bytes, sizeof (t));
  __builtin_memcpy (&v, v_bytes, sizeof (v));
  if (get_u (t) != 0x123456)
    __builtin_abort ();
  if (get_returned_u (&t) != 0x123456)
    __builtin_abort ();
  if (get_s (v) != 0x923456789a - 0x10000000000)
    __builtin_abort ();
#endif
}
