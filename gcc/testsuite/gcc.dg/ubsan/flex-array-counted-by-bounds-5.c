/* The bounds sanitizer doesn't use a counted_by count with reverse scalar
   storage order.  Reading the count with its bytes swapped gave a bound of
   1 for a count of 256 and flagged an access that is in bounds.  */
/* { dg-do run } */
/* { dg-options "-fsanitize=bounds -fno-sanitize-recover=bounds" } */

#include <stdlib.h>

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define REVERSE "big-endian"
#else
#define REVERSE "little-endian"
#endif

struct __attribute__ ((scalar_storage_order (REVERSE))) flex
{
  unsigned short count;
  char data[] __attribute__ ((counted_by (count)));
};

struct __attribute__ ((scalar_storage_order (REVERSE))) ptr
{
  unsigned short count;
  char *data __attribute__ ((counted_by (count)));
};

__attribute__ ((noipa)) void
test (struct flex *f, struct ptr *p, int index)
{
  f->data[index] = 1;
  p->data[index] = 2;
}

int
main (void)
{
  struct flex *f = (struct flex *) malloc (sizeof (struct flex) + 256);
  struct ptr *p = (struct ptr *) malloc (sizeof (struct ptr));

  f->count = 256;
  p->count = 256;
  p->data = (char *) malloc (256);
  test (f, p, 1);
  return 0;
}
