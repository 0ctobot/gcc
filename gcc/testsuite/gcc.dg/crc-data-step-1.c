/* A CRC loop must shift the data by exactly one bit per iteration, towards
   the bit the condition reads.  These loops skip or drop data bits, so they
   aren't CRC computations.  */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-crc-details" } */
/* { dg-require-effective-target int32plus } */

/* Only data bits 0, 2, 4 and 6 reach the condition.  */
unsigned int
shift2_rev (unsigned int crc, unsigned char data)
{
  for (int i = 0; i < 8; i++)
    {
      if ((crc ^ data) & 1)
	crc = (crc >> 1) ^ 0xEDB88320u;
      else
	crc >>= 1;
      data >>= 2;
    }
  return crc;
}

/* Only data bits 7, 5, 3 and 1 reach the condition.  */
unsigned short
shift2_fwd (unsigned short crc, unsigned char data)
{
  for (int i = 0; i < 8; i++)
    {
      if ((crc ^ (data << 8)) & 0x8000)
	crc = (unsigned short) ((crc << 1) ^ 0x1021);
      else
	crc = (unsigned short) (crc << 1);
      data <<= 2;
    }
  return crc;
}

/* Data bits 6 and 7 never reach the condition.  */
unsigned int
masked_rev (unsigned int crc, unsigned char data)
{
  for (int i = 0; i < 8; i++)
    {
      if ((crc ^ data) & 1)
	crc = (crc >> 1) ^ 0xEDB88320u;
      else
	crc >>= 1;
      data = (data >> 1) & 0x3f;
    }
  return crc;
}

/* The data shifts away from the bit the condition reads, so only data
   bit 0 reaches it.  */
unsigned short
wrongdir_rev (unsigned short crc, unsigned char data)
{
  for (int i = 0; i < 8; i++)
    {
      if ((crc ^ data) & 1)
	crc = (crc >> 1) ^ 0xA001;
      else
	crc >>= 1;
      data <<= 1;
    }
  return crc;
}

/* { dg-final { scan-tree-dump-times "Data isn't shifted by one bit per iteration" 4 "crc" } } */
/* { dg-final { scan-tree-dump-not "calculates CRC" "crc" } } */
