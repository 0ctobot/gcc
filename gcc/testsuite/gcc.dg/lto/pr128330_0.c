/* PR lto/128330 */
/* -fstack-protector guards functions holding a character array, and a plain
   char array must count as one under LTO too, where plain char doesn't come
   back from streaming as char_type_node.  */
/* { dg-lto-do run } */
/* { dg-require-effective-target fstack_protector } */
/* { dg-lto-options { { -O2 -flto -fstack-protector } { -O2 -flto -fstack-protector -fsigned-char } { -O2 -flto -fstack-protector -funsigned-char } } } */

extern void exit (int);

/* The call to this only appears at expand time, so LTO must be told to keep it
   or the failing guard reaches the C library's version instead.  */
__attribute__ ((used, externally_visible)) void
__stack_chk_fail (void)
{
  exit (0); /* pass */
}

__attribute__ ((noipa)) void
fill (char *p, int n)
{
  __builtin_memset (p, 42, n);
}

__attribute__ ((noipa)) void
victim (int n)
{
  char buf[16];
  fill (buf, n);
}

int
main (void)
{
  victim (48);
  return 1; /* fail */
}
