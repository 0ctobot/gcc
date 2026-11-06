/* { dg-do run { target aarch64_sve2048_hw } } */
/* { dg-options "-O2 -msve-vector-bits=2048" } */

#include "vec_perm_2_generic.c"

int
main (void)
{
  static v256qi r, x, y, z;
  for (int i = 0; i < 256; i++)
    {
      x[i] = i;
      y[i] = 0x80 + i;
      z[i] = 255 - i;
    }
  f (&r, &x, &y, &z);
  for (int i = 0; i < 256; i++)
    if (r[i] != (unsigned char) (255 - i))
      __builtin_abort ();
  return 0;
}
