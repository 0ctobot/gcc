/* { dg-do run } */
/* { dg-require-effective-target int32plus } */

/* Starting at -8 and shifting in ones makes the two low bits match
   whenever bit zero is set.  The filter therefore contributes no bits.  */

__attribute__ ((noipa)) unsigned int
filtered (unsigned int crc)
{
  signed char data = -8;
  for (int i = 0; i < 8; ++i)
    {
      unsigned int u = data;
      if ((crc ^ (u & ~(u >> 1))) & 1)
	crc = (crc >> 1) ^ 0xedb88320U;
      else
	crc >>= 1;
      data = (int) ((u & 65535U) >> 1) - 32768;
    }
  return crc;
}

__attribute__ ((noipa)) unsigned int
unfiltered (unsigned int crc)
{
  signed char data = -8;
  for (int i = 0; i < 8; ++i)
    {
      unsigned int u = data;
      if ((crc ^ u) & 1)
	crc = (crc >> 1) ^ 0xedb88320U;
      else
	crc >>= 1;
      data = (int) ((u & 65535U) >> 1) - 32768;
    }
  return crc;
}

int
main (void)
{
  if (filtered (0) != 0 || unfiltered (0) != 0xb3667a2eU)
    __builtin_abort ();
  return 0;
}
