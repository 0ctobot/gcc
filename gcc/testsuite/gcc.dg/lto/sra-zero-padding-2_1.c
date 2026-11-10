/* { dg-options "-fzero-init-padding-bits=all" } */

/* The alignment of cookie leaves padding after type on every target.  */
struct pad
{
  char type;
  long cookie __attribute__ ((aligned (8)));
  short flags;
};

void
fill (struct pad *dst, char type, long cookie, short flags)
{
  struct pad r = {};

  r.type = type;
  r.cookie = cookie;
  r.flags = flags;
  *dst = r;
}
