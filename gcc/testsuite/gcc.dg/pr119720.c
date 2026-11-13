/* PR tree-optimization/119720 */
/* With -fno-strict-aliasing, reading p->a doesn't imply that the rest of
   *p exists, so p->b must not be read when c is nonzero.  */
/* { dg-do run { target { { *-*-linux* *-*-gnu* *-*-uclinux* } && mmap } } } */
/* { dg-options "-O2 -fno-strict-aliasing -fhoist-adjacent-loads --param l1-cache-line-size=64" } */

#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

struct s { int a; int b; };

__attribute__((noipa)) int
f (int c, struct s *p)
{
  int t;
  if (c)
    t = p->a;
  else
    t = p->b;
  return t;
}

int
main ()
{
  long pgsz = sysconf (_SC_PAGESIZE);
  if (pgsz < (long) sizeof (int))
    return 0;
  char *p = mmap ((void *) 0, 2 * pgsz, PROT_NONE,
		  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (p == MAP_FAILED)
    return 0;
  if (mprotect (p, pgsz, PROT_READ | PROT_WRITE))
    return 0;
  int *q = (int *) (p + pgsz) - 1;
  *q = 42;
  if (f (1, (struct s *) q) != 42)
    abort ();
  return 0;
}
