/* Narrowing an unsigned value that doesn't fit wraps it, which can leave an
   offset with its top bit set the same way a subtraction can, so a pointer
   that's offset by it may be below the start of the subobject.  A narrowed
   value that's known to fit keeps the subobject's size.  */
/* { dg-do run { target int128 } } */
/* { dg-options "-O2 -Wno-array-bounds" } */

typedef __SIZE_TYPE__ size_t;
typedef unsigned __int128 u128;

struct pkt { char tag[8]; char label[24]; unsigned w[3]; long z; };

struct pkt tbl[5];

#define NOIPA __attribute__ ((noipa))
#define L tbl[1].label
#define END ((char *) (tbl + 5))
#define BIG (((u128) 1) << 65)
#define MASK ((((u128) 1) << 64) + (u128) (size_t) -8)

NOIPA size_t
min_big (u128 x)
{
  return __builtin_object_size (L + (size_t) (x < BIG ? x : BIG), 1);
}

NOIPA size_t
and_big (u128 x)
{
  return __builtin_object_size (L + (size_t) (x & MASK), 1);
}

NOIPA size_t
cvt (u128 x)
{
  return __builtin_object_size (L + (size_t) x, 1);
}

NOIPA size_t
and_small (u128 x)
{
  return __builtin_object_size (L + (size_t) (x & 15), 1);
}

NOIPA size_t
shift (u128 x)
{
  return __builtin_object_size (L + (size_t) (x >> 68), 1);
}

#define CHECK_LEFT(sz, p) \
  do { if ((sz) < (size_t) (END - (char *) (p))) __builtin_abort (); } while (0)

int
main (void)
{
  /* Each of these offsets is (size_t) -8.  */
  CHECK_LEFT (min_big (BIG - 8), L - 8);
  CHECK_LEFT (and_big (MASK), L - 8);
  CHECK_LEFT (cvt (MASK), L - 8);

  if (and_small (MASK) != 24 || shift (MASK) != 24)
    __builtin_abort ();

  return 0;
}
