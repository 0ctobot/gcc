/* PR tree-optimization/128196 */
/* { dg-do run } */

/* strnlen returns min (strlen, bound), so its result isn't the length of a
   string whose leading bytes are the only thing the strlen pass knows about.
   The pass used to record it as the full length, which made a later strlen
   return the bound, gave the result a lower bound that a smaller bound could
   violate, and treated the byte at the result as the terminating nul.  */

/* The prefix comes from memcpy over a longer string.  */

__attribute__((noipa)) unsigned long
f1 (const char *s)
{
  char b[16];
  __builtin_strcpy (b, s);
  __builtin_memcpy (b, "abc", 3);
  unsigned long n = __builtin_strnlen (b, 5);
  return n * 100 + __builtin_strlen (b);
}

/* Likewise, without the runtime string.  */

__attribute__((noipa)) unsigned long
f2 (void)
{
  char b[16];
  __builtin_memcpy (b, "0123456789", 11);
  __builtin_memcpy (b, "abc", 3);
  unsigned long n = __builtin_strnlen (b, 5);
  return n * 100 + __builtin_strlen (b);
}

/* The prefix comes from byte stores.  */

__attribute__((noipa)) unsigned long
f3 (const char *s)
{
  char b[16];
  __builtin_strcpy (b, s);
  b[0] = 'a';
  b[1] = 'b';
  b[2] = 'c';
  unsigned long n = __builtin_strnlen (b, 5);
  return n * 100 + __builtin_strlen (b);
}

/* A bound smaller than the prefix means the result can be smaller than the
   prefix too.  */

__attribute__((noipa)) int
f4 (const char *s, unsigned long m)
{
  char b[16];
  __builtin_strcpy (b, s);
  __builtin_memcpy (b, "abc", 3);
  unsigned long n = __builtin_strnlen (b, m);
  return n < 3;
}

/* The byte at the result isn't necessarily the terminating nul.  */

__attribute__((noipa)) int
f5 (const char *s)
{
  char b[16];
  __builtin_strcpy (b, s);
  __builtin_memcpy (b, "abc", 3);
  unsigned long n = __builtin_strnlen (b, 5);
  char *e = b + n;
  return *e == 0;
}

/* strnlen of the tail must not update the length of the whole string.  */

__attribute__((noipa)) unsigned long
f6 (const char *s)
{
  char b[32];
  __builtin_memset (b, 'z', 31);
  b[31] = 0;
  __builtin_strcpy (b, s);
  char *p = b + __builtin_strlen (b);
  __builtin_memcpy (p, "xy", 2);
  unsigned long n = __builtin_strnlen (p, 3);
  return n * 100 + __builtin_strlen (b);
}

/* strcpy must copy the whole string, not just the bytes strnlen counted.  */

__attribute__((noipa)) unsigned long
f7 (char *d, const char *s)
{
  char b[16];
  __builtin_strcpy (b, s);
  __builtin_memcpy (b, "abc", 3);
  unsigned long n = __builtin_strnlen (b, 5);
  __builtin_strcpy (d, b);
  return n;
}

/* strchr must find the real end of the string.  */

__attribute__((noipa)) unsigned long
f8 (const char *s)
{
  char b[16];
  __builtin_strcpy (b, s);
  __builtin_memcpy (b, "abc", 3);
  unsigned long n = __builtin_strnlen (b, 5);
  return n * 100 + (__builtin_strchr (b, 0) - b);
}

int
main (void)
{
  char d[16];

  if (f1 ("0123456789") != 510)
    __builtin_abort ();
  if (f2 () != 510)
    __builtin_abort ();
  if (f3 ("0123456789") != 510)
    __builtin_abort ();
  if (f4 ("0123456789", 1) != 1)
    __builtin_abort ();
  if (f5 ("0123456789") != 0)
    __builtin_abort ();
  if (f6 ("abc") != 331)
    __builtin_abort ();
  if (f7 (d, "0123456789") != 5 || __builtin_strcmp (d, "abc3456789") != 0)
    __builtin_abort ();
  if (f8 ("0123456789") != 510)
    __builtin_abort ();
  return 0;
}
