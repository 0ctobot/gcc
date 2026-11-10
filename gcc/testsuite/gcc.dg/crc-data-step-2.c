/* Genuine CRC loops whose data advances through conversions around a
   one-bit shift, including sign extension and arithmetic right shifts of
   signed data, must still be recognized.  So must bit-forward loops that
   step the data with data += data or data *= 2, which reach the pass as a
   multiply by 2.  */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-crc-details" } */
/* { dg-require-effective-target int32plus } */

unsigned int
rev_signed (unsigned int crc, signed char data)
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

unsigned short
fwd_signed (unsigned short crc, signed char data)
{
  for (int i = 0; i < 8; i++)
    {
      if ((crc ^ (data << 8)) & 0x8000)
	crc = (unsigned short) ((crc << 1) ^ 0x1021);
      else
	crc = (unsigned short) (crc << 1);
      data <<= 1;
    }
  return crc;
}

unsigned short
fwd_unsigned (unsigned short crc, unsigned char data)
{
  for (int i = 0; i < 8; i++)
    {
      if ((crc ^ (data << 8)) & 0x8000)
	crc = (unsigned short) ((crc << 1) ^ 0x1021);
      else
	crc = (unsigned short) (crc << 1);
      data <<= 1;
    }
  return crc;
}

unsigned short
fwd_add (unsigned short crc, unsigned char data)
{
  for (int i = 0; i < 8; i++)
    {
      if ((crc ^ (data << 8)) & 0x8000)
	crc = (unsigned short) ((crc << 1) ^ 0x1021);
      else
	crc = (unsigned short) (crc << 1);
      data += data;
    }
  return crc;
}

unsigned short
fwd_mul (unsigned short crc, unsigned char data)
{
  for (int i = 0; i < 8; i++)
    {
      if ((crc ^ (data << 8)) & 0x8000)
	crc = (unsigned short) ((crc << 1) ^ 0x1021);
      else
	crc = (unsigned short) (crc << 1);
      data *= 2;
    }
  return crc;
}

/* { dg-final { scan-tree-dump-times "calculates CRC!" 5 "crc" } } */
/* { dg-final { scan-tree-dump-not "Data isn't shifted by one bit per iteration" "crc" } } */
