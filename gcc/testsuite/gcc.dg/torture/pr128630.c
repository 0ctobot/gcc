/* PR middle-end/128630 */
/* { dg-do run } */
/* { dg-additional-options "-Wno-psabi" } */

typedef unsigned char V __attribute__((vector_size (256)));

#define B(n) (n) & 255, (n + 1) & 255, (n + 2) & 255, (n + 3) & 255
#define C(n) B (n), B (n + 4), B (n + 8), B (n + 12)
#define D(n) C (n), C (n + 16), C (n + 32), C (n + 48)

/* The selector steps by 1 and wraps in its 8-bit element type, so lane 255
   is 0 and selects lane 0 of the first input.  */
#define MASK { D (1), D (65), D (129), D (193) }

__attribute__((noipa)) V
fold (void)
{
  V z = {}, m = MASK;
  return __builtin_shuffle (z + 7, z, m);
}

__attribute__((noipa)) V
perm (V a, V b)
{
  V m = MASK;
  return __builtin_shuffle (a, b, m);
}

static int
sel (V a, V b, int m)
{
  m &= 511;
  return m < 256 ? a[m] : b[m - 256];
}

int
main (void)
{
  V z = {}, m = MASK, a, b, f, p;

  for (int i = 0; i < 256; i++)
    {
      a[i] = i;
      b[i] = 255 - i;
    }

  f = fold ();
  p = perm (a, b);
  for (int i = 0; i < 256; i++)
    {
      if (f[i] != sel (z + 7, z, m[i]))
	__builtin_abort ();
      if (p[i] != sel (a, b, m[i]))
	__builtin_abort ();
    }
  return 0;
}
