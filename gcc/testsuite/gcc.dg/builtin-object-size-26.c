/* Folding can turn container_of on a variable array element and a member
   access into a single non-constant offset from the element.  A dynamic maximum
   subobject size must not take that for a pointer leaving the array, since it
   lands in another member of the same object with room left.  */
/* { dg-do run } */
/* { dg-options "-O2" } */

#include "builtin-object-size-common.h"

#define DOS __builtin_dynamic_object_size
#define NI __attribute__ ((noipa))
#define container_of(p, T, f) ((T *) ((char *) (p) - __builtin_offsetof (T, f)))

struct item
{
  long x;
  int idx;
};

struct ring
{
  long h;
  struct item items[4];
  char tag[32];
};

static struct ring gr;

NI __attribute__ ((alloc_size (1))) void *
get (__SIZE_TYPE__ sz)
{
  return __builtin_malloc (sz);
}

/* The same index both ways.  */
NI long
same_index (int i)
{
  struct item *it = &gr.items[i];
  struct ring *q = container_of (it, struct ring, items[i]);
  return DOS (q->tag + 4, 1);
}

/* Two names for the same index.  */
NI long
two_indexes (int i, int j)
{
  struct item *it = &gr.items[i];
  struct ring *q = container_of (it, struct ring, items[j]);
  return DOS (q->tag + 4, 1);
}

NI long
two_indexes_alloc (int i, int j)
{
  struct ring *r = get (sizeof *r);
  struct item *it = &r->items[i];
  struct ring *q = container_of (it, struct ring, items[j]);
  return DOS (q->tag + 4, 1);
}

/* A constant offset from a variable element.  */
NI long
constant_index (int i)
{
  struct item *it = &gr.items[i];
  struct ring *q = container_of (it, struct ring, items[i]);
  it = &q->items[2];
  q = container_of (it, struct ring, items[2]);
  return DOS (q->tag + 4, 1);
}

/* A variable offset out of the array has the rest of the object to go by, while
   one that stays in the array is still held to the array.  */
NI long
step (long off)
{
  char *p = (char *) gr.items;
  return DOS (p + off, 1);
}

int
main (void)
{
  if (same_index (2) != sizeof (gr.tag) - 4)
    FAIL ();
  if (two_indexes (2, 2) != sizeof (gr.tag) - 4)
    FAIL ();
  if (two_indexes_alloc (2, 2) != sizeof (gr.tag) - 4)
    FAIL ();
  if (constant_index (2) != sizeof (gr.tag) - 4)
    FAIL ();
  if (step (8) != sizeof (gr.items) - 8)
    FAIL ();
  if (step (sizeof (gr.items) + 4) != sizeof (gr.tag) - 4)
    FAIL ();

  DONE ();
}
