/* A switch in a loop whose cases each run an inline asm that emits 40
   instructions, which GCC's length estimate counts as three.  Byte jump
   table entries can't reach the later case labels.  */
/* { dg-do run } */
/* { dg-options "-O2" } */

#define NOPS asm volatile (".rept 40\nnop\n.endr")

__attribute__ ((noipa)) long
f (const unsigned char *p, long n, long a)
{
  long r = 0;
  while (n--)
    {
      switch (*p++)
	{
	case 0: NOPS; r += a * 3; break;
	case 1: NOPS; r ^= a * 5; break;
	case 2: NOPS; r += a * 7; continue;
	case 3: NOPS; r -= a * 9; break;
	case 4: NOPS; r += a * 11; continue;
	case 5: NOPS; r ^= a * 13; break;
	case 6: NOPS; r += a * 15; break;
	case 7: NOPS; r -= a * 17; continue;
	case 8: NOPS; r += a * 19; break;
	case 9: NOPS; r ^= a * 21; break;
	case 10: NOPS; r += a * 23; break;
	case 11: NOPS; r -= a * 25; continue;
	default: NOPS; return r;
	}
      r += 1;
    }
  return r;
}

__attribute__ ((noipa, optimize ("O0"))) long
ref (const unsigned char *p, long n, long a)
{
  static const int m[12] = { 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25 };
  long r = 0;
  while (n--)
    {
      unsigned c = *p++;
      if (c > 11)
	return r;
      if (c == 1 || c == 5 || c == 9)
	r ^= a * m[c];
      else if (c == 3 || c == 7 || c == 11)
	r -= a * m[c];
      else
	r += a * m[c];
      if (c == 2 || c == 4 || c == 7 || c == 11)
	continue;
      r += 1;
    }
  return r;
}

int
main (void)
{
  unsigned char s[64];
  for (int k = 0; k < 12; k++)
    {
      for (int i = 0; i < 64; i++)
	s[i] = (i * 7 + k * 5) % 12;
      s[63] = 12;
      if (f (s, 64, k + 2) != ref (s, 64, k + 2))
	__builtin_abort ();
    }
  return 0;
}
