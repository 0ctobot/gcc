/* PR tree-optimization/114206 */
/* Tail recursion must not reuse the frame of an activation when the next one
   can tell where the local of the previous one lives, even if the bits only
   get there through branches on the address.  */
/* { dg-do run } */
/* { dg-require-effective-target lp64 } */
/* { dg-options "-O2" } */

extern void abort (void);

__attribute__ ((noinline)) int
rebuilt_bits (int n, unsigned int lo)
{
  int local = n;
  unsigned long me = (unsigned long) &local;
  unsigned int piece = 0;

  for (int i = 0; i < 32; i++)
    if (me & (1ul << i))
      piece |= 1u << i;
  if (lo == piece)
    return -1;
  if (n == 0)
    return 0;
  return rebuilt_bits (n - 1, piece);
}

__attribute__ ((noinline)) int
rebuilt_bits_else (int n, unsigned int lo)
{
  int local = n;
  unsigned int a = (unsigned int) (unsigned long) &local, v = 0;

  if (n == 0)
    return a == lo ? -1 : 0;
  for (int i = 0; i < 32; i++)
    {
      if (a & (1u << i))
	v |= 1u << i;
      else
	__asm__ ("");
    }
  return rebuilt_bits_else (n - 1, v);
}

int
main (void)
{
  if (rebuilt_bits (3, 0) != 0 || rebuilt_bits_else (1, 0) != 0)
    abort ();
  return 0;
}
