/* A variable offset followed by a negative constant through a member is a
   valid way to find a header in front of a position in a buffer.  The static
   maximum presumes the variable offset is 0, so the negative constant must not
   be taken as an underflow that yields a size of 0.  */
/* { dg-do run } */
/* { dg-options "-O2" } */

#include "builtin-object-size-common.h"

struct hdr
{
  int len;
  char name[16];
  int tail;
};

char gbuf[256];

#define NI __attribute__ ((noipa))
#define HDR(buf, n) \
  ({ char *pos = (char *) (buf) + (n); \
     (struct hdr *) (pos - sizeof (struct hdr)); })

/* The position is 100 bytes into 256 bytes, so the name member of the header
   that ends there has 256 - 100 + 24 - 4 = 176 bytes left in the object.  */
#define LEFT 176

NI __attribute__ ((alloc_size (1))) void *
get (__SIZE_TYPE__ sz)
{
  return __builtin_malloc (sz);
}

NI void
check_alloc (long n)
{
  char *buf = get (256);
  struct hdr *h = HDR (buf, n);

  if (__builtin_object_size (h->name, 0) < LEFT)
    FAIL ();
  if (__builtin_object_size (h->name, 1) < sizeof (h->name))
    FAIL ();
  if (__builtin_dynamic_object_size (h->name, 0) != LEFT)
    FAIL ();
  if (__builtin_dynamic_object_size (h->name, 1) != sizeof (h->name))
    FAIL ();
}

NI void
check_global (long n)
{
  struct hdr *h = HDR (gbuf, n);

  if (__builtin_object_size (h->name, 0) < LEFT)
    FAIL ();
  if (__builtin_object_size (h->name, 1) < sizeof (h->name))
    FAIL ();
  if (__builtin_dynamic_object_size (h->name, 0) != LEFT)
    FAIL ();
  if (__builtin_dynamic_object_size (h->name, 1) != sizeof (h->name))
    FAIL ();
}

NI void
check_local (long n)
{
  char buf[256];
  struct hdr *h = HDR (buf, n);

  __asm__ ("" : : : "memory");
  if (__builtin_object_size (h->name, 0) < LEFT)
    FAIL ();
  if (__builtin_object_size (h->name, 1) < sizeof (h->name))
    FAIL ();
  if (__builtin_dynamic_object_size (h->name, 0) != LEFT)
    FAIL ();
  if (__builtin_dynamic_object_size (h->name, 1) != sizeof (h->name))
    FAIL ();
}

/* A second constant step on top of the member stays a valid position.  */
NI void
check_step (long n)
{
  char *buf = get (256);
  struct hdr *h = HDR (buf, n);

  if (__builtin_object_size (h->name + 3, 0) < LEFT - 3)
    FAIL ();
  if (__builtin_object_size (h->name + 3, 1) < sizeof (h->name) - 3)
    FAIL ();
  if (__builtin_dynamic_object_size (h->name + 3, 0) != LEFT - 3)
    FAIL ();
  if (__builtin_dynamic_object_size (h->name + 3, 1) < sizeof (h->name) - 3)
    FAIL ();
}

/* Without a variable offset the position is known and the underflow is real,
   so the size stays 0.  */
NI void
check_constant (void)
{
  struct hdr *h = HDR (gbuf, 8);

  if (__builtin_object_size (h->name, 0) != 0)
    FAIL ();
  if (__builtin_dynamic_object_size (h->name, 0) != 0)
    FAIL ();
}

int
main (void)
{
  check_alloc (100);
  check_global (100);
  check_local (100);
  check_step (100);
  check_constant ();

  DONE ();
}
