/* Loops whose condition mixes in another bit of the register instead of a
   data bit must keep their own result.  ok_crc32 is a genuine CRC.  */
/* { dg-do run } */
/* { dg-require-effective-target int32plus } */

#define NI __attribute__ ((noipa))

/* Genuine reflected CRC-32 over one byte.  */
NI unsigned int
ok_crc32 (unsigned int crc, unsigned char data)
{
  for (int i = 0; i < 8; i++)
    {
      if ((crc ^ data) & 1)
	crc = (crc >> 1) ^ 0xEDB88320u;
      else
	crc >>= 1;
      data >>= 1;
    }
  return crc;
}

/* The condition mixes in bit 3 of the register instead of a data bit.  */
NI unsigned int
crcxor_rev (unsigned int crc)
{
  for (int i = 0; i < 8; i++)
    {
      if ((crc ^ (crc >> 3)) & 1)
	crc = (crc >> 1) ^ 0xEDB88320u;
      else
	crc >>= 1;
    }
  return crc;
}

/* The condition mixes in bit 13 of the register instead of a data bit.  */
NI unsigned short
crcxor_fwd (unsigned short crc)
{
  for (int i = 0; i < 16; i++)
    {
      if ((crc ^ (crc << 2)) & 0x8000)
	crc = (unsigned short) ((crc << 1) ^ 0x1021);
      else
	crc = (unsigned short) (crc << 1);
    }
  return crc;
}

static const unsigned int crcs[3] = { 0, 0x12345678u, 0xffffffffu };
static const unsigned char datas[3] = { 2, 0xa5, 0x80 };

int
main (void)
{
  static const unsigned int ok_crc32_exp[3] =
    { 0xee0e612c, 0xf8709a3f, 0xc0459352 };
  static const unsigned int crcxor_rev_exp[3] =
    { 0, 0x7d15aae7, 0xffffff };
  static const unsigned int crcxor_fwd_exp[3] =
    { 0, 0x1569, 0x3063 };

  for (int k = 0; k < 3; k++)
    {
      if (ok_crc32 (crcs[k], datas[k]) != ok_crc32_exp[k])
	__builtin_abort ();
      if (crcxor_rev (crcs[k]) != crcxor_rev_exp[k])
	__builtin_abort ();
      if (crcxor_fwd ((unsigned short) crcs[k]) != crcxor_fwd_exp[k])
	__builtin_abort ();
    }
  return 0;
}
