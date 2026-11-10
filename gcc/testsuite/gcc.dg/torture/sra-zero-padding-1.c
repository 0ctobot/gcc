/* { dg-do run } */
/* { dg-additional-options "-ftrivial-auto-var-init=zero" } */

/* With -ftrivial-auto-var-init=zero, the padding of a local aggregate reads
   as zero, and a copy of the whole local carries those zeros along.  The
   destinations start out filled with 0xa5, so a copy that only stores the
   fields leaves 0xa5 in their padding.  */

struct pad { char type; long cookie; short flags; };

struct pad buf;

__attribute__((noipa)) void
dirty (void)
{
  volatile unsigned char b[256];
  int i;

  for (i = 0; i < 256; i++)
    b[i] = 0xa5;
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

__attribute__((noipa)) void *
get_buf (void)
{
  return &buf;
}

/* A local built field by field and copied into memory through a
   pointer.  */
__attribute__((noipa)) void
fill (char type, long cookie, short flags)
{
  struct pad r;
  struct pad *dst = get_buf ();

  r.type = type;
  r.cookie = cookie;
  r.flags = flags;
  *dst = r;
}

/* A copy of a local into another local whose address escapes.  */
__attribute__((noipa)) int
send_copy (char type, long cookie, short flags)
{
  struct pad tmp;

  tmp.type = type;
  tmp.cookie = cookie;
  tmp.flags = flags;
  struct pad out = tmp;
  return padding_dirty (&out);
}

int
main (void)
{
  __builtin_memset (&buf, 0xa5, sizeof buf);
  fill (1, 2, 3);
  if (buf.type != 1 || buf.cookie != 2 || buf.flags != 3)
    __builtin_abort ();
  if (padding_dirty (&buf))
    __builtin_abort ();
  dirty ();
  if (send_copy (1, 2, 3))
    __builtin_abort ();
  return 0;
}
