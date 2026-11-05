/* PR tree-optimization/128238 */
/* { dg-do run } */

/* A memcpy from a source with only a few known nonzero leading bytes must not
   leave the destination with its old string length, because the rest of the
   copied bytes are unknown.  */

#define LEN 46

char g[LEN + 1];
char src[16] = "?bcdefgh";
char src_nul[16] = "?b\0defgh";
char dst[16];

/* The form from the bug report.  */

__attribute__((noipa)) __SIZE_TYPE__
report (void)
{
  char buf[LEN + 1];
#pragma GCC novector
  for (int i = 0; i < LEN; ++i)
    buf[i] = i ? 'y' : 'x';
  buf[LEN] = 0;

  char buf2[LEN + 1];
  __builtin_memcpy (buf2, buf, LEN + 1);

  char buf3[LEN + 1] = { 0 };
  __builtin_memcpy (buf3, buf2, LEN);
  buf3[LEN] = 0;

  return __builtin_strlen (buf3);
}

/* The whole-array copy leaves BUF2 with no known nonzero bytes, and BUF3 starts
   out as an empty string.  */

__attribute__((noipa)) __SIZE_TYPE__
whole_array (void)
{
  char buf2[LEN + 1];
  char buf3[LEN + 1] = { 0 };
  __builtin_memcpy (buf2, g, LEN + 1);
  __builtin_memcpy (buf3, buf2, LEN);
  return __builtin_strlen (buf3);
}

/* The copy replaces the old nul with nonzero bytes, making the string longer.
   The condition keeps DSE from deleting the strcpy.  */

__attribute__((noipa)) __SIZE_TYPE__
longer (char *restrict d, char *restrict s, int c)
{
  __builtin_strcpy (d, "xy");
  s[0] = 'a';
  if (c)
    {
      __builtin_memcpy (d, s, 7);
      return __builtin_strlen (d);
    }
  return 0;
}

/* The copy stays within the old string and writes a nul, making the string
   shorter.  */

__attribute__((noipa)) __SIZE_TYPE__
shorter (char *restrict d, char *restrict s, int c)
{
  __builtin_strcpy (d, "abcdefghij");
  s[0] = 'a';
  if (c)
    {
      __builtin_memcpy (d, s, 7);
      return __builtin_strlen (d);
    }
  return 0;
}

/* The string at D is related to the one at P, so it loses its length too.  */

__attribute__((noipa)) __SIZE_TYPE__
related (char *restrict d, char *restrict s, int c)
{
  __builtin_strcpy (d, "abcd");
  char *p = d + 2;
  __SIZE_TYPE__ len = __builtin_strlen (p);
  s[0] = 'a';
  if (c)
    {
      __builtin_memcpy (p, s, 7);
      return __builtin_strlen (d) * 100 + __builtin_strlen (p) * 10 + len;
    }
  return 0;
}

int
main (void)
{
  if (report () != LEN)
    __builtin_abort ();

  __builtin_memset (g, 'y', LEN);
  if (whole_array () != LEN)
    __builtin_abort ();

  if (longer (dst, src, 1) != 7)
    __builtin_abort ();

  __builtin_memset (dst, 0, sizeof dst);
  if (shorter (dst, src_nul, 1) != 2)
    __builtin_abort ();

  __builtin_memset (dst, 0, sizeof dst);
  if (related (dst, src, 1) != 972)
    __builtin_abort ();

  return 0;
}
