/* { dg-do run { target { aarch64_sve_hw && aarch64*-*-linux* } } } */
/* { dg-options "-O3 -fno-strict-overflow -fno-strict-aliasing -fno-vect-cost-model -msve-vector-bits=2048" } */

/* The loop runs for 256 iterations but its count is a wrapped 8-bit value, which
   is also the VF with 2048-bit vectors.  Peeling for alignment leaves too few
   iterations for a vector iteration whenever the data is misaligned, but the
   sum of the prolog and epilog iterations wrapped in the check for that.  */

#include <sys/prctl.h>

#ifndef PR_SVE_SET_VL
#define PR_SVE_SET_VL 50
#endif
#ifndef PR_SVE_VL_LEN_MASK
#define PR_SVE_VL_LEN_MASK 0xffff
#endif

typedef unsigned char u8;
typedef unsigned int u32;

__attribute__ ((noipa)) long
f (u32 *p, u8 *b, u8 s, u8 e)
{
  long x = 0;
  do
    {
      if (b[x] == 0xee)
	break;
      p[x] += 2;
      x++;
    }
  while (++s != e);
  return x;
}

static u32 pp[2000];
static u8 bb[2000] __attribute__ ((aligned (256)));

int
main (void)
{
  /* The code generation assumes 2048-bit vectors.  */
  int vl = prctl (PR_SVE_SET_VL, 256);
  if (vl < 0 || (vl & PR_SVE_VL_LEN_MASK) != 256)
    return 0;

  for (int off = 0; off < 8; off++)
    {
      for (int i = 0; i < 2000; i++)
	{
	  pp[i] = 0;
	  bb[i] = 1;
	}
      if (f (pp + off, bb + off, 0, 0) != 256)
	__builtin_abort ();
      for (int i = 0; i < 2000; i++)
	if (pp[i] != (i >= off && i < off + 256 ? 2 : 0))
	  __builtin_abort ();
    }
  return 0;
}
