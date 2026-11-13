/* PR tree-optimization/128511 */
/* { dg-do run } */
/* { dg-require-effective-target int32plus } */

/* The product of an affine evolution and one whose step itself evolves in
   the same loop was folded as if both steps were invariant, which gave
   wrong final values.  Sums below use sum (i^2, i < 100) = 328350,
   sum (i^3, i < 100) = 24502500, and for t = i * i,
   t * (t + i) = i^4 + i^3.  */

__attribute__((noipa)) int
f1 (void)
{
  int r = 0;
  for (int i = 0; i < 100; i++)
    r += i * i - i * i * i;
  return r;
}

__attribute__((noipa)) unsigned
f2 (void)
{
  unsigned r = 0;
  for (unsigned i = 0; i < 100; i++)
    r += i * i - i * i * i;
  return r;
}

__attribute__((noipa)) int
f3 (void)
{
  int r = 0;
  for (int i = 0; i < 100; i++)
    r += (1 - i) * (i * i);
  return r;
}

__attribute__((noipa)) int
f4 (void)
{
  int r = 0;
  for (int i = 0; i < 100; i++)
    {
      int s = i - 1;
      r += (i * i) * s;
    }
  return r;
}

/* sum ((2 - 2i) * (i^2 + 3i + 5), i < 100).  */

__attribute__((noipa)) int
f5 (void)
{
  int r = 0;
  for (int i = 0; i < 100; i++)
    r += (2 - 2 * i) * (i * i + 3 * i + 5);
  return r;
}

/* sum ((i^2 + i + 1) * (4 - 4i), i < 100).  */

__attribute__((noipa)) int
f6 (void)
{
  int r = 0;
  for (int i = 0; i < 100; i++)
    {
      int q = i * i + i + 1;
      r += q * (4 - 4 * i);
    }
  return r;
}

/* sum ((1 - i) * (65536 i^2 + 7), i < 100) = -1584277128350, reduced
   modulo 2^32.  */

__attribute__((noipa)) unsigned
f7 (void)
{
  unsigned r = 0;
  for (unsigned i = 0; i < 100; i++)
    r += (1 - i) * (i * i * 65536 + 7);
  return r;
}

/* Products of two evolutions of degree two.  */

__attribute__((noipa)) int
f8 (void)
{
  int r = 0;
  for (int i = 0; i < 50; i++)
    {
      int t = i * i;
      r += t * (t + i);
    }
  return r;
}

/* sum (i^4 + i^3, i < 1000) = 199749833583300, reduced modulo 2^32.  */

__attribute__((noipa)) unsigned
f9 (void)
{
  unsigned r = 0;
  for (unsigned i = 0; i < 1000; i++)
    {
      unsigned t = i * i;
      r += t * (t + i);
    }
  return r;
}

int
main (void)
{
  if (f1 () != 328350 - 24502500)
    __builtin_abort ();
  if (f2 () != 328350u - 24502500u)
    __builtin_abort ();
  if (f3 () != 328350 - 24502500)
    __builtin_abort ();
  if (f4 () != 24502500 - 328350)
    __builtin_abort ();
  if (f5 () != -50337200)
    __builtin_abort ();
  if (f6 () != -98009600)
    __builtin_abort ();
  if (f7 () != (unsigned) -1584277128350LL)
    __builtin_abort ();
  if (f8 () != 60917290)
    __builtin_abort ();
  if (f9 () != (unsigned) 199749833583300LL)
    __builtin_abort ();
  return 0;
}
