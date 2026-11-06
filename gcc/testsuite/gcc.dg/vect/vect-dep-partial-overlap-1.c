/* The stride is a multiple of the size of the structure, but the flexible array
   reaches into the structure of the next iteration, so each load has to see the
   store from the iteration before it.  */
/* { dg-require-effective-target int32plus } */

#include "tree-vect.h"

/* may_alias keeps the test independent of type based alias analysis.  */
struct __attribute__ ((may_alias)) flex { int n; int d[]; };

#define N 64

static unsigned char a[4096] __attribute__ ((aligned (16)));
static unsigned char e[4096] __attribute__ ((aligned (16)));

__attribute__ ((noipa)) static void
foo (unsigned char *p)
{
  for (long i = 0; i < N; i++)
    {
      struct flex *f = (struct flex *) (p + 12 * i);
      f->d[3] = f->d[0] + 1;
    }
}

__attribute__ ((noipa)) static void
foo_ref (unsigned char *p)
{
  for (long i = 0; i < N; i++)
    {
      volatile unsigned char *src = p + 12 * i + 4;
      volatile unsigned char *dst = p + 12 * i + 16;
      unsigned char t[sizeof (int)];
      int v;
      for (unsigned int k = 0; k < sizeof (int); k++)
	t[k] = src[k];
      __builtin_memcpy (&v, t, sizeof (int));
      v = (unsigned int) v + 1;
      __builtin_memcpy (t, &v, sizeof (int));
      for (unsigned int k = 0; k < sizeof (int); k++)
	dst[k] = t[k];
    }
}

int
main (void)
{
  check_vect ();

  for (int i = 0; i < 4096; i++)
    a[i] = e[i] = i * 37 + 11 + (i >> 3);
  foo (a + 2048);
  foo_ref (e + 2048);
  if (__builtin_memcmp (a, e, sizeof (a)))
    __builtin_abort ();
  return 0;
}
