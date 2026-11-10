/* { dg-do run } */

/* The IOR rule for single bit bitfields in simplify_binary_operation_1
   must not merge (ne (and X Y) 0) into the mask when Y isn't a constant.
   It read the register rtx as a number and tested X against that garbage
   mask instead of X & Y, so b2 and b4 never called h for 0x400.  */

int cnt;

__attribute__((noipa)) void
h (void)
{
  cnt++;
}

__attribute__((noipa)) void
b2 (unsigned long x, unsigned long y)
{
  if (((x >> 3) | ((x & y) != 0)) & 1)
    h ();
}

__attribute__((noipa)) void
b4 (unsigned long v, unsigned long y)
{
  unsigned long x = v & 0x408;
  if (((x >> 3) | ((x & y) != 0)) & 1)
    h ();
}

int
main (void)
{
  /* x & y is nonzero and bit 3 of x is clear: h must be called.  */
  cnt = 0;
  b2 (0x400, 0x400);
  if (cnt != 1)
    __builtin_abort ();
  cnt = 0;
  b4 (0x400, 0x400);
  if (cnt != 1)
    __builtin_abort ();

  /* Bit 3 of x is set: h must be called.  */
  cnt = 0;
  b2 (0x8, 0);
  if (cnt != 1)
    __builtin_abort ();

  /* Neither: h must not be called.  */
  cnt = 0;
  b2 (0x10, 0);
  if (cnt != 0)
    __builtin_abort ();
  cnt = 0;
  b4 (0x400, 0x8);
  if (cnt != 0)
    __builtin_abort ();
  return 0;
}
