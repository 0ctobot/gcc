/* PR tree-optimization/127964 */
/* { dg-do run } */
/* { dg-options "-fdump-tree-crc-details" } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-flto" } } */
/* { dg-require-effective-target int32plus } */

/* The inner loop leaves through the true edge of its exit condition.  The CRC
   pass used to force that condition to false after replacing the loop with an
   internal function call, which made the loop spin forever.  Marking the
   function cold keeps the loop from being unrolled before the CRC pass at
   -O3.  */

__attribute__ ((noipa, cold)) __UINT32_TYPE__
crc32_bytes (__UINT32_TYPE__ crc, const unsigned char *p, unsigned int n)
{
  for (unsigned int k = 0; k < n; k++)
    {
      crc ^= p[k];
      unsigned int j = 0;
      for (;;)
	{
	  if (crc & 1)
	    crc = (crc >> 1) ^ 0xEDB88320u;
	  else
	    crc = crc >> 1;
	  j++;
	  if (j == 8)
	    break;
	}
    }
  return crc;
}

int
main (void)
{
  static const unsigned char msg[] = "123456789";

  if (crc32_bytes (0xFFFFFFFFu, msg, 9) != ~0xcbf43926u)
    __builtin_abort ();

  return 0;
}

/* { dg-final { scan-tree-dump "calculates CRC!" "crc" } } */
