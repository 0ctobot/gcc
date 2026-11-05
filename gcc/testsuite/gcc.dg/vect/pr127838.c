/* { dg-add-options vect_early_break } */
/* { dg-require-effective-target vect_early_break_hw } */
/* { dg-require-effective-target vect_int } */
/* { dg-require-effective-target mmap } */

#include <sys/mman.h>
#include <unistd.h>
#include "tree-vect.h"

/* A LENGTH of -16 makes the scalar iteration count close to 2^64, so its
   product with the step of S wraps to a small negative value, and the run-time
   alias test must not conclude from that that S and RES are independent.  The
   first store to RES changes S[1], which makes the loop stop right away.  The
   guard page turns a vector loop that keeps going into a fault.  */

__attribute__ ((noipa)) void
f (const unsigned *s, char *res, int length)
{
  unsigned long i;

  for (i = 0; i < length; i++)
    {
      if (s[i] > 255)
	return;
      res[i] = (char) s[i];
    }
}

int
main (void)
{
  long pgsz = sysconf (_SC_PAGESIZE);
  char *p;
  unsigned *s;
  int i;

  check_vect ();

  p = mmap (NULL, pgsz * 2, PROT_READ | PROT_WRITE,
	    MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  if (p == MAP_FAILED)
    return 0;
  mprotect (p + pgsz, pgsz, PROT_NONE);

  s = (unsigned *) (p + pgsz - 64);
  s[0] = 1;
  for (i = 1; i < 16; i++)
    s[i] = 0;

  /* The first store to RES writes 1 to the byte of S[1] worth 0x100, which
     is at offset 5 on a little-endian target and offset 6 on a big-endian
     one.  That makes S[1] 256 on either byte order and stops the loop.  */
  f (s, (char *) s + (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ ? 5 : 6),
     -16);

  if (s[1] <= 255 || s[2] != 0)
    __builtin_abort ();
  return 0;
}
