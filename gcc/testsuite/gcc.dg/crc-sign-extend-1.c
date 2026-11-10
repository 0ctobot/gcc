/* Bit-serial loops that sign-extend their register into a wider temporary
   and then shift the temporary right bring copies of the sign bit into the
   register, so they aren't CRCs and must not be replaced.  */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-crc-details" } */
/* { dg-require-effective-target int32plus } */

signed char
s8_int (signed char crc, unsigned char data)
{
  crc ^= data;
  for (int i = 0; i < 8; i++)
    {
      int t = crc;
      if (t & 1)
	t = (t >> 1) ^ 0x8C;
      else
	t >>= 1;
      crc = (signed char) t;
    }
  return crc;
}

unsigned char
u8_cast (unsigned char crc, unsigned char data)
{
  crc ^= data;
  for (int i = 0; i < 8; i++)
    {
      int t = (signed char) crc;
      if (t & 1)
	t = (t >> 1) ^ 0x8C;
      else
	t >>= 1;
      crc = t;
    }
  return crc;
}

short
s16_int (short crc, unsigned char data)
{
  crc ^= data;
  for (int i = 0; i < 8; i++)
    {
      int x = crc;
      if (x & 1)
	x = (x >> 1) ^ 0xA001;
      else
	x >>= 1;
      crc = x;
    }
  return crc;
}

int
s32_ll (int crc, unsigned char data)
{
  crc ^= data;
  for (int i = 0; i < 8; i++)
    {
      long long t = crc;
      if (t & 1)
	crc = (t >> 1) ^ 0xEDB88320;
      else
	crc = t >> 1;
    }
  return crc;
}

int
s32_ll32 (int crc, unsigned int data)
{
  crc ^= data;
  for (int i = 0; i < 32; i++)
    {
      long long x = crc;
      crc = x >> 1;
      if (x & 1)
	crc ^= 0xEDB88320;
    }
  return crc;
}

/* { dg-final { scan-tree-dump-not "calculates CRC" "crc" } } */
