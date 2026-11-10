/* A memcpy within one buffer whose destination covers the nul of its
   source string must drop the length the strlen pass knew for that
   string.  */
/* { dg-do run } */
/* { dg-options "-O2" } */

typedef __SIZE_TYPE__ size_t;
extern void *memcpy (void *, const void *, size_t);
extern void *memmove (void *, const void *, size_t);
extern size_t strlen (const char *);
extern char *strcpy (char *, const char *);
extern int strcmp (const char *, const char *);
extern void *memset (void *, int, size_t);

__attribute__((noipa)) int
opq (int x)
{
  return x;
}

/* The destination offset isn't constant: memcpy's own ranges don't
   overlap, but the destination covers the source string's nul.  */

__attribute__((noipa)) size_t
var_off (int i)
{
  char b[16] = "abcdef";
  memcpy (b + i, b, 3);
  return strlen (b);
}

/* The wrong length would turn the strcpy into a memcpy that drops the real
   nul.  */

__attribute__((noipa)) void
to_strcpy (int i, char *d)
{
  char b[16] = "abcdef";
  memcpy (b + i, b, 3);
  strcpy (d, b);
}

/* memmove with a constant offset.  */

__attribute__((noipa)) void
const_off (size_t *o)
{
  char x0[9] = { 0 };
  strcpy (x0, "\x7a\x7f\x7f\x79\x78\x78");
  o[0] = strlen (x0 + 5);
  memmove (x0 + 4, x0 + 0, 3);
  o[1] = strlen (x0 + 0);
}

int
main (void)
{
  char d[32];
  size_t o[2];

  if (var_off (opq (4)) != 7)
    __builtin_abort ();
  memset (d, 'Z', sizeof d - 1);
  d[31] = 0;
  to_strcpy (opq (4), d);
  if (strcmp (d, "abcdabc") != 0)
    __builtin_abort ();
  const_off (o);
  if (o[0] != 1 || o[1] != 7)
    __builtin_abort ();
  return 0;
}
