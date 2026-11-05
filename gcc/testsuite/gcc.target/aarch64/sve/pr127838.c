/* { dg-do run { target aarch64_sve_hw } } */
/* { dg-options "-O3 -fno-strict-overflow" } */

/* The constant iteration count times the step of S wraps to 0 in sizetype,
   which used to make the vectorizer conclude at compile time that S and RES
   are independent.  The first store to RES changes S[1], which stops the
   loop.  */

__attribute__ ((noipa)) void
f (char *buf)
{
  const unsigned *s = (const unsigned *) buf;
  char *res = buf + (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ ? 5 : 6);

  for (unsigned long i = 0; i < 0x4000000000000001UL; i++)
    {
      if (s[i] > 255)
	return;
      res[i] = s[i];
    }
}

int
main (void)
{
  _Alignas (16) unsigned s[64] = { 1, 0, 7 };

  s[63] = 1000;
  f ((char *) s);
  if (s[1] != 256 || s[2] != 7)
    __builtin_abort ();
  return 0;
}
