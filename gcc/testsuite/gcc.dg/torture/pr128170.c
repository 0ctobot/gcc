/* PR tree-optimization/128170 */
/* { dg-do run } */

/* The strlen pass must not keep a string length across a store that may
   truncate the string.  It used to mark the string as exempt from the next
   clobber check while handling a statement without a VDEF, and that exemption
   then applied to the next store, or to the walk back from the virtual PHI at
   a loop header.  */

char buf[16];
char *bufp = buf;

/* strlen of a suffix installs the suffix's length and adjusts the length of
   the string it's part of.  */

__attribute__((noipa)) unsigned long
suffix (char *b, char *q)
{
  b[0] = 'x';
  b[1] = 'y';
  unsigned long n = __builtin_strlen (b + 2);
  *q = 0;
  return n * 1000 + __builtin_strlen (b);
}

__attribute__((noipa)) unsigned long
suffix_branch (char *b, char *q, int c)
{
  b[0] = 'x';
  b[1] = 'y';
  unsigned long n = __builtin_strlen (b + 2);
  if (c)
    {
      *q = 0;
      return n * 1000 + __builtin_strlen (b);
    }
  return n;
}

__attribute__((noipa)) unsigned long
suffix_loop (char *b, char *q, int k)
{
  b[0] = 'x';
  b[1] = 'y';
  unsigned long r = __builtin_strlen (b + 2);
  for (int i = 0; i < k; i++)
    {
      r = r * 1000 + __builtin_strlen (b);
      *q = 0;
    }
  return r;
}

/* An alloc_size function that's discovered to be const or pure has no VDEF
   either.  */

__attribute__((noinline, noclone, alloc_size (1))) static char *
get_const (unsigned long n)
{
  return buf;
}

__attribute__((noinline, noclone, alloc_size (1))) static char *
get_pure (unsigned long n)
{
  return bufp;
}

__attribute__((noipa)) unsigned long
alloc_const (char *q)
{
  char *p = get_const (16);
  unsigned long n = __builtin_strlen (p);
  *q = 0;
  return n * 1000 + __builtin_strlen (p);
}

__attribute__((noipa)) unsigned long
alloc_pure (char *q)
{
  char *p = get_pure (16);
  unsigned long n = __builtin_strlen (p);
  *q = 0;
  return n * 1000 + __builtin_strlen (p);
}

int
main (void)
{
  __builtin_strcpy (buf, "0123456789");
  if (suffix (buf, buf + 2) != 8 * 1000 + 2)
    __builtin_abort ();

  __builtin_strcpy (buf, "0123456789");
  if (suffix_branch (buf, buf + 2, 1) != 8 * 1000 + 2)
    __builtin_abort ();

  __builtin_strcpy (buf, "0123456789");
  if (suffix_loop (buf, buf + 2, 2) != (8 * 1000 + 10) * 1000 + 2)
    __builtin_abort ();

  __builtin_strcpy (buf, "0123456789");
  if (alloc_const (buf + 2) != 10 * 1000 + 2)
    __builtin_abort ();

  __builtin_strcpy (buf, "0123456789");
  if (alloc_pure (buf + 2) != 10 * 1000 + 2)
    __builtin_abort ();

  return 0;
}
