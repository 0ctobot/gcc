/* PR target/54412 */
/* { dg-do run } */
/* { dg-options "-O2 -Wno-psabi" } */
/* { dg-require-effective-target alloca } */

/* An over-aligned homogeneous vector aggregate is passed in SIMD registers
   until they run out and then in a stack slot aligned to only 16 bytes.  Every
   form must reach the function body in a properly aligned object once its
   address is taken.  */

#include <arm_neon.h>

struct H { float32x4_t v[4]; } __attribute__ ((aligned (64)));

int fails;

__attribute__ ((noipa)) void
check (const void *p)
{
  if ((unsigned long) p & 63)
    fails++;
}

__attribute__ ((noipa)) struct H
ret (float x)
{
  struct H h;
  for (int i = 0; i < 4; i++)
    h.v[i] = vdupq_n_f32 (x + i);
  check (&h);
  return h;
}

__attribute__ ((noipa)) float
by_value (struct H h)
{
  check (&h);
  return vgetq_lane_f32 (h.v[1], 0);
}

__attribute__ ((noipa)) float
on_stack (struct H a, struct H b, struct H c)
{
  check (&a);
  check (&b);
  check (&c);
  return vgetq_lane_f32 (c.v[0], 0);
}

__attribute__ ((noipa)) float
use (float x)
{
  struct H a = ret (x);
  return by_value (a) + on_stack (a, ret (2), ret (x + 3));
}

int
main (void)
{
  for (int i = 1; i <= 8; i++)
    {
      char *volatile p = __builtin_alloca (16 * i);
      p[0] = 0;
      if (use (i) != i + 1 + i + 3)
	__builtin_abort ();
    }
  if (fails)
    __builtin_abort ();
  return 0;
}
