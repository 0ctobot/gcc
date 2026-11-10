/* Genuine CRC loops on signed registers must still be recognized: a
   forward loop shifts the bits that the sign extension adds away from the
   register, and a reflected loop that zero-extends the register never
   sees its sign bit copied.  So must a forward loop that sign-extends its
   data, since the copies of the data's sign bit land above the register.  */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-crc-details" } */
/* { dg-require-effective-target int32plus } */

signed char
fwd_s8 (signed char crc, unsigned char data)
{
  crc ^= data;
  for (int i = 0; i < 8; i++)
    {
      int t = crc;
      if (t & 0x80)
	t = (t << 1) ^ 0x07;
      else
	t <<= 1;
      crc = (signed char) t;
    }
  return crc;
}

short
rev_s16_zext (short crc, unsigned char data)
{
  crc ^= data;
  for (int i = 0; i < 8; i++)
    {
      int t = (unsigned short) crc;
      if (t & 1)
	t = (t >> 1) ^ 0xA001;
      else
	t >>= 1;
      crc = (short) t;
    }
  return crc;
}

int
rev_s32_zext (int crc, unsigned char data)
{
  crc ^= data;
  for (int i = 0; i < 8; i++)
    {
      unsigned long long t = (unsigned int) crc;
      if (t & 1)
	crc = (t >> 1) ^ 0xEDB88320;
      else
	crc = t >> 1;
    }
  return crc;
}

unsigned short
fwd_signed_data (unsigned short crc, signed char data)
{
  crc ^= data << 8;
  for (int i = 0; i < 8; i++)
    {
      if (crc & 0x8000)
	crc = (crc << 1) ^ 0x1021;
      else
	crc <<= 1;
    }
  return crc;
}

/* { dg-final { scan-tree-dump-times "calculates CRC!" 4 "crc" } } */
