/* Reflected CRC loops that convert a signed register to a wider type before
   shifting it: with a cast to a wider unsigned type in the shift expression,
   with a temporary whose shifted value is stored back before its low bit is
   tested, and with a temporary that the data is XORed into inside the loop.
   The conversion sign-extends the register, so the shift leaves its top bit
   set when it's negative instead of clearing it, and none of these loops
   computes a CRC.  */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-crc-details" } */
/* { dg-require-effective-target int32plus } */

signed char
rev_s8_ushort (signed char crc)
{
  for (int i = 0; i < 8; i++)
    if (crc & 1)
      crc = ((unsigned short) crc >> 1) ^ (signed char) 0x8C;
    else
      crc = (unsigned short) crc >> 1;
  return crc;
}

short
rev_shift_first (short crc, unsigned char data)
{
  crc ^= data;
  for (int i = 0; i < 8; i++)
    {
      int t = crc;
      crc = t >> 1;
      if (t & 1)
	crc ^= 0xA001u;
    }
  return crc;
}

signed char
rev_data_in_loop (signed char crc, signed char data)
{
  for (int i = 0; i < 8; i++)
    {
      int t = crc;
      if ((t ^ data) & 1)
	t = (t >> 1) ^ 0x8C;
      else
	t >>= 1;
      crc = t;
      data >>= 1;
    }
  return crc;
}

/* { dg-final { scan-tree-dump-not "calculates CRC" "crc" } } */
