/* { dg-lto-do run } */
/* { dg-lto-options {{-O2 -flto} {-O2 -flto -flto-partition=none}} } */

/* fill is compiled with -fzero-init-padding-bits=all, but the link isn't.
   The copy of its zero-initialized local must still write the zeroed
   padding into the destination after link-time optimization, the same as
   it does without LTO.  */

/* The alignment of cookie leaves padding after type on every target.  */
struct pad
{
  char type;
  long cookie __attribute__ ((aligned (8)));
  short flags;
};

extern void fill (struct pad *, char, long, short);

struct pad buf;

__attribute__((noipa)) int
padding_dirty (const void *p)
{
  const unsigned char *c = p;
  unsigned long i;
  int bad = 0;

  for (i = 0; i < sizeof (struct pad); i++)
    if (i >= __builtin_offsetof (struct pad, type) + 1
	&& i < __builtin_offsetof (struct pad, cookie))
      bad |= c[i];
    else if (i >= __builtin_offsetof (struct pad, flags) + sizeof (short))
      bad |= c[i];
  return bad;
}

int
main (void)
{
  __builtin_memset (&buf, 0xa5, sizeof buf);
  fill (&buf, 1, 2, 3);
  if (buf.type != 1 || buf.cookie != 2 || buf.flags != 3)
    __builtin_abort ();
  if (padding_dirty (&buf))
    __builtin_abort ();
  return 0;
}
