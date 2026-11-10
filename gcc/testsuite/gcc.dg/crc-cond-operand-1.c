/* The CRC's condition must xor the register's significant bit with the
   data's next bit, or test the significant bit alone when there's no data.
   These loops mix in another bit of the register instead, so they aren't
   CRC computations.  */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-crc-details" } */
/* { dg-require-effective-target int32plus } */

/* The condition mixes in bit 3 of the register instead of a data bit.  */
unsigned int
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
unsigned short
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

/* { dg-final { scan-tree-dump-times "Returned state and LFSR differ" 2 "crc" } } */
/* { dg-final { scan-tree-dump-not "calculates CRC" "crc" } } */
