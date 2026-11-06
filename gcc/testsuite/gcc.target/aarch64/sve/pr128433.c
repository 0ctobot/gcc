/* PR tree-optimization/128433 */
/* { dg-do compile { target mmap } } */
/* { dg-options "-O3 -msve-vector-bits=256" } */

#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

__attribute__ ((noipa)) void
f (const unsigned int *s, unsigned int *d, long n)
{
  for (long i = 0; i < n; ++i)
    {
      d[2 * i] = s[i];
      d[2 * i + 1] = s[i];
    }
}

/* The mask for the two elements stored per iteration must not be used for the
   load, which only reads one element per iteration.  */
/* { dg-final { scan-assembler-not {\tld1w\tz[0-9]+\.s, p[0-7]/z, \[x0} } } */

int
main (void)
{
  long page = sysconf (_SC_PAGESIZE);
  char *memory = mmap (0, 2 * page, PROT_READ | PROT_WRITE,
		       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (memory == MAP_FAILED || mprotect (memory + page, page, PROT_NONE))
    abort ();

  /* Put the only source element right before an inaccessible page.  */
  volatile long count = 1;
  unsigned int *s = (unsigned int *) (memory + page - count * sizeof (*s));
  static unsigned int d[64];
  s[0] = 1;
  f (s, d, count);
  if (d[0] != 1 || d[1] != 1 || d[2] != 0)
    abort ();
  return 0;
}
