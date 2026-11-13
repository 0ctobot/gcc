/* The counted_by attribute is ignored for a count with reverse scalar storage
   order, which can only be read correctly through the field itself.
   __builtin_dynamic_object_size falls back to the size of the object and
   __builtin_counted_by_ref gives a null pointer, instead of both using the
   count with its bytes swapped.  A count with the native storage order still
   gives the size, also when the array is in a struct with reverse storage
   order.  */
/* { dg-do run } */
/* { dg-skip-if "mixed endian" { ! { le || be } } } */
/* { dg-options "-O2 -Wno-scalar-storage-order" } */

#include "builtin-object-size-common.h"

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define NATIVE "little-endian"
#define REVERSE "big-endian"
#else
#define NATIVE "big-endian"
#define REVERSE "little-endian"
#endif

struct __attribute__ ((scalar_storage_order (REVERSE))) rev
{
  unsigned int count;
  int data[] __attribute__ ((counted_by (count)));
};

struct __attribute__ ((scalar_storage_order (NATIVE))) nat
{
  unsigned int count;
  int data[] __attribute__ ((counted_by (count)));
};

/* The storage order of the count comes from the anonymous struct that holds
   it, not from the enclosing struct.  */
struct rev_anon
{
  struct __attribute__ ((scalar_storage_order (REVERSE)))
  {
    unsigned int count;
  };
  int data[] __attribute__ ((counted_by (count)));
};

struct __attribute__ ((scalar_storage_order (REVERSE))) nat_anon
{
  struct
  {
    unsigned int count;
  };
  int data[] __attribute__ ((counted_by (count)));
};

struct __attribute__ ((scalar_storage_order (REVERSE))) rev_ptr
{
  unsigned int count;
  int *data __attribute__ ((counted_by (count)));
};

#define SIZE(S, MODE)						\
  __attribute__ ((noipa)) size_t				\
  S##_size##MODE (struct S *p)					\
  {								\
    return __builtin_dynamic_object_size (p->data, MODE);	\
  }

SIZE (rev, 0)
SIZE (rev, 1)
SIZE (nat, 0)
SIZE (nat, 1)
SIZE (rev_anon, 0)
SIZE (rev_anon, 1)
SIZE (nat_anon, 0)
SIZE (nat_anon, 1)
SIZE (rev_ptr, 0)
SIZE (rev_ptr, 1)

static int rev_buf[16], nat_buf[16], rev_anon_buf[16], nat_anon_buf[16];
static int ptr_buf[8];
static struct rev_ptr rp;

int
main (void)
{
  struct rev *r = (struct rev *) rev_buf;
  struct nat *n = (struct nat *) nat_buf;
  struct rev_anon *ra = (struct rev_anon *) rev_anon_buf;
  struct nat_anon *na = (struct nat_anon *) nat_anon_buf;
  size_t rest = sizeof (rev_buf) - __builtin_offsetof (struct rev, data);

  r->count = 4;
  n->count = 4;
  ra->count = 4;
  na->count = 4;
  rp.count = 4;
  rp.data = ptr_buf;

  EXPECT (rev_size0 (r), -1);
  EXPECT (rev_size1 (r), -1);
  EXPECT (nat_size0 (n), 4 * sizeof (int));
  EXPECT (nat_size1 (n), 4 * sizeof (int));
  EXPECT (rev_anon_size0 (ra), -1);
  EXPECT (rev_anon_size1 (ra), -1);
  EXPECT (nat_anon_size0 (na), 4 * sizeof (int));
  EXPECT (nat_anon_size1 (na), 4 * sizeof (int));
  EXPECT (rev_ptr_size0 (&rp), -1);
  EXPECT (rev_ptr_size1 (&rp), -1);

  /* Without the count, the size of a known object is still available.  */
  EXPECT (__builtin_dynamic_object_size (r->data, 0), rest);
  EXPECT (__builtin_dynamic_object_size (r->data, 1), rest);
  EXPECT (__builtin_object_size (r->data, 0), rest);
  EXPECT (__builtin_object_size (r->data, 1), rest);

  if (__builtin_counted_by_ref (r->data)
      || __builtin_counted_by_ref (ra->data)
      || __builtin_counted_by_ref (rp.data))
    FAIL ();
  if (__builtin_counted_by_ref (n->data) != &n->count
      || __builtin_counted_by_ref (na->data) != &na->count)
    FAIL ();

  DONE ();
}
