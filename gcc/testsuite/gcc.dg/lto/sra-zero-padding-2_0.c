/* { dg-lto-do run } */
/* { dg-lto-options {{-O2 -flto} {-O2 -flto -flto-partition=none}} } */

/* This test covers the case where fill can be inlined.  fill is compiled with
   -fzero-init-padding-bits=all, but the link isn't, and fill isn't noipa, so
   it is a candidate for inlining into main.  Inlining it into a function
   built with a different setting must be refused, since the inlined body
   would be optimized under main's setting.  The copy of its zero-initialized
   local must still write the zeroed padding into the destination after
   link-time optimization, the same as it does without LTO.  scribble does
   the memset out of line, since a memset in main is deleted as dead once
   fill is inlined and its whole-object copy covers buf.  */

/* The alignment of cookie leaves padding after type on every target.  */
struct pad
{
  char type;
  long cookie __attribute__ ((aligned (8)));
  short flags;
};

extern void fill (struct pad *, char, long, short);

struct pad buf;

__attribute__((noipa)) void
scribble (void *p)
{
  __builtin_memset (p, 0xa5, sizeof (struct pad));
}

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
  scribble (&buf);
  fill (&buf, 1, 2, 3);
  if (buf.type != 1 || buf.cookie != 2 || buf.flags != 3)
    __builtin_abort ();
  if (padding_dirty (&buf))
    __builtin_abort ();
  return 0;
}
