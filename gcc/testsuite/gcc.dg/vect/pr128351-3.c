/* { dg-require-effective-target vect_int } */
/* { dg-additional-options "-O3" } */

/* An iteration count of 0 stands for 2^N when the counter wraps.  Groups as wide
   as the vector give a vectorization factor of 1, which leaves the vector loop
   with the same wrapped count as the scalar one.  */

#include "tree-vect.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

#define NUM_U16 65536

static u32 a[(NUM_U16 + 1) * 4] __attribute__ ((aligned (64)));

#define DEF(C)						\
  __attribute__ ((noipa)) u32 *				\
  inc_##C (u32 *p, C n)					\
  {							\
    C i = 0;						\
    do							\
      {							\
	p[0] += 1;					\
	p[1] += 2;					\
	p[2] += 3;					\
	p[3] += 4;					\
	p += 4;						\
      }							\
    while (++i != n);					\
    return p;						\
  }

DEF (u8)
DEF (u16)

__attribute__ ((noipa)) void
check (u32 *end, unsigned int count)
{
  unsigned int i;

  if (end != a + count * 4)
    abort ();
  for (i = 0; i < (NUM_U16 + 1) * 4; i++)
    {
      if (a[i] != (i < count * 4 ? i % 4 + 1 : 0))
	abort ();
      a[i] = 0;
    }
}

int
main (void)
{
  check_vect ();

  check (inc_u8 (a, 0), 256);
  check (inc_u8 (a, 3), 3);
  check (inc_u8 (a, 255), 255);
  check (inc_u16 (a, 0), NUM_U16);
  check (inc_u16 (a, 3), 3);
  check (inc_u16 (a, 65535), 65535);
  return 0;
}

/* { dg-final { scan-tree-dump "LOOP VECTORIZED" "vect" { target aarch64*-*-* } } } */
