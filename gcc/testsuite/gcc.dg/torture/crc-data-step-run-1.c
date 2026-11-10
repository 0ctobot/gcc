/* Loops that don't shift the data by exactly one bit per iteration must
   keep their own result.  ok_crc32 and ok_fwd_add are genuine CRCs.  */
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

/* Genuine CRC-16 whose data advances by adding it to itself.  */
NI unsigned short
ok_fwd_add (unsigned short crc, unsigned char data)
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

/* Only data bits 0, 2, 4 and 6 reach the condition.  */
NI unsigned int
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
NI unsigned short
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
NI unsigned int
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
NI unsigned short
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

/* The condition reads bit 0 of a counter that starts at 4 and steps by 2,
   which is always 0, so this is a CRC-16 of the register alone.  Passing
   the counter's initial value as the data would mix in its bit 2.  This
   is the loop from crc-29.c.  */
NI unsigned short
counter_step2 (unsigned short crc)
{
  unsigned char carry;
  for (unsigned char i = 0, j = 4; i < 8; i++, j += 2)
    {
      if ((unsigned char) ((unsigned char) crc & 1) ^ (j & 1))
	{
	  crc ^= 0x4002;
	  carry = 1;
	}
      else
	carry = 0;
      crc >>= 1;
      if (carry)
	crc |= 0x8000;
      else
	crc &= 0x7fff;
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
  static const unsigned int ok_fwd_add_exp[3] =
    { 0x2042, 0xa77c, 0x7078 };
  static const unsigned int shift2_rev_exp[3] =
    { 0, 0xc7c59ce2, 0x2dfd1072 };
  static const unsigned int shift2_fwd_exp[3] =
    { 0x1231, 0x9b7f, 0x7078 };
  static const unsigned int masked_rev_exp[3] =
    { 0xee0e612c, 0x15c8191f, 0x2dfd1072 };
  static const unsigned int wrongdir_rev_exp[3] =
    { 0, 0xe297, 0x40bf };
  static const unsigned int counter_step2_exp[3] =
    { 0, 0x2256, 0x40bf };

  for (int k = 0; k < 3; k++)
    {
      if (ok_crc32 (crcs[k], datas[k]) != ok_crc32_exp[k])
	__builtin_abort ();
      if (ok_fwd_add ((unsigned short) crcs[k], datas[k]) != ok_fwd_add_exp[k])
	__builtin_abort ();
      if (shift2_rev (crcs[k], datas[k]) != shift2_rev_exp[k])
	__builtin_abort ();
      if (shift2_fwd ((unsigned short) crcs[k], datas[k]) != shift2_fwd_exp[k])
	__builtin_abort ();
      if (masked_rev (crcs[k], datas[k]) != masked_rev_exp[k])
	__builtin_abort ();
      if (wrongdir_rev ((unsigned short) crcs[k], datas[k])
	  != wrongdir_rev_exp[k])
	__builtin_abort ();
      if (counter_step2 ((unsigned short) crcs[k]) != counter_step2_exp[k])
	__builtin_abort ();
    }
  return 0;
}
