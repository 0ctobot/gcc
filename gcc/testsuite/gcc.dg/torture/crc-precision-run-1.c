/* CRC loops on a register whose type is narrower than its mode, a 12-bit
   bit-field or a small _BitInt, lose the bits that the shift moves above
   the type's precision.  Each must compute the same values as its ref_
   copy, which keeps the register in an unsigned int, masks it to the
   type's precision, and has an iteration count that the CRC pass can't
   see.  */
/* { dg-do run } */
/* { dg-require-effective-target int32plus } */

#define NI __attribute__ ((noipa))

volatile int n8 = 8;

struct bf { unsigned c : 12; unsigned d : 4; };

NI unsigned
bf12 (unsigned c, unsigned char data)
{
  struct bf b;
  b.c = c;
  b.c ^= data;
  for (int i = 0; i < 8; i++)
    {
      if (b.c & 1)
	b.c = (b.c >> 1) ^ 0xA01;
      else
	b.c = b.c >> 1;
    }
  return b.c;
}

NI unsigned
ref_rev (unsigned crc, unsigned char data, unsigned poly, unsigned mask)
{
  crc = (crc ^ data) & mask;
  for (int i = 0; i < n8; i++)
    {
      if (crc & 1)
	crc = ((crc >> 1) ^ poly) & mask;
      else
	crc >>= 1;
    }
  return crc;
}

#ifdef __BITINT_MAXWIDTH__
typedef unsigned _BitInt(12) u12;
typedef unsigned _BitInt(13) u13;

NI u12
bitint12 (u12 crc, unsigned char data)
{
  crc ^= data;
  for (int i = 0; i < 8; i++)
    {
      if (crc & 1)
	crc = (crc >> 1) ^ (u12) 0xA01;
      else
	crc >>= 1;
    }
  return crc;
}

/* The data is XORed in inside the loop.  */
NI u13
bitint13 (u13 crc, unsigned char data)
{
  for (int i = 0; i < 8; i++)
    {
      if ((crc ^ data) & 1)
	crc = (crc >> 1) ^ (u13) 0x1401;
      else
	crc >>= 1;
      data >>= 1;
    }
  return crc;
}
#endif

int
main (void)
{
  for (unsigned c = 0; c < 0x2000; c += 7)
    for (unsigned d = 0; d < 0x100; d += 3)
      {
	if (bf12 (c & 0xFFF, d) != ref_rev (c, d, 0xA01, 0xFFF))
	  __builtin_abort ();
#ifdef __BITINT_MAXWIDTH__
	if ((unsigned) bitint12 ((u12) c, d) != ref_rev (c, d, 0xA01, 0xFFF))
	  __builtin_abort ();
	if ((unsigned) bitint13 ((u13) c, d) != ref_rev (c, d, 0x1401, 0x1FFF))
	  __builtin_abort ();
#endif
      }

  return 0;
}
