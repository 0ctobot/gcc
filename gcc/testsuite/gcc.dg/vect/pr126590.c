/* { dg-require-effective-target vect_int } */

#include "tree-vect.h"

/* The unroll pragma can make the vectorization factor a non-power of two.
   Since these do-while loops leave niters free to wrap, the vector loop's
   bound comes from (niters - vf) / vf + 1, whose range must be computed by
   dividing by the factor rather than shifting by its log2.  */

__attribute__((noipa)) void
f (unsigned int *a, unsigned int n)
{
  unsigned int i = 0;
  unsigned int *p = a;
#pragma GCC unroll 12
  do
    *p++ += 1;
  while (++i != n);
}

/* N from 252 to 255 gives the vector loop its largest possible bound of 21.  */
__attribute__((noipa)) void
g (unsigned int *a, unsigned char n)
{
  unsigned char i = 0;
  unsigned int *p = a;
#pragma GCC unroll 12
  do
    *p++ += 1;
  while (++i != n);
}

static unsigned int a[1024];

static void
check (unsigned int n)
{
  for (unsigned int i = 0; i < 1024; i++)
    {
      if (a[i] != (i < n))
	__builtin_abort ();
      a[i] = 0;
    }
}

int
main (void)
{
  check_vect ();

  for (unsigned int n = 1; n <= 1000; n++)
    {
      f (a, n);
      check (n);
    }

  for (unsigned int n = 1; n <= 255; n++)
    {
      g (a, n);
      check (n);
    }

  return 0;
}

/* { dg-final { scan-tree-dump-times "using 16 byte vectors and unroll factor 12" 2 "vect" { target { aarch64*-*-* && { ! aarch64_sve } } } } } */
