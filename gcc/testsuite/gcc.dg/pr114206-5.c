/* PR tree-optimization/114206 */
/* Every activation has its own copy of an aggregate parameter, which tail
   recursion can only merge if no activation can tell.  */
/* { dg-do run } */
/* { dg-require-effective-target lp64 } */
/* { dg-options "-O2" } */

extern void abort (void);

struct b { long v[4]; };
struct s { int v; };

/* The next activation reads the copy of its caller through a pointer.  */
__attribute__ ((noinline)) long
through_pointer (struct b a, int n, struct b *prev)
{
  a.v[0] = n + 1000;
  long seen = prev ? prev->v[0] : 0;
  if (n == 0)
    return seen;
  return through_pointer (a, n - 1, &a);
}

__attribute__ ((noinline)) int
through_integer (struct s a, int n, unsigned long prev)
{
  unsigned long me = (unsigned long) &a;
  if (prev == me)
    return -1;
  if (n == 0)
    return 0;
  return through_integer (a, n - 1, me);
}

__attribute__ ((noinline)) int
through_branches (struct s a, int n, unsigned int lo)
{
  unsigned long me = (unsigned long) &a;
  unsigned int piece = 0;

  for (int i = 0; i < 32; i++)
    if (me & (1ul << i))
      piece |= 1u << i;
  if (lo == piece)
    return -1;
  if (n == 0)
    return 0;
  return through_branches (a, n - 1, piece);
}

int
main (void)
{
  struct b z = { { 7, 0, 0, 0 } };
  struct s y = { 1 };

  if (through_pointer (z, 1, 0) != 1001)
    abort ();
  if (through_integer (y, 3, 0) != 0)
    abort ();
  if (through_branches (y, 3, 0) != 0)
    abort ();
  return 0;
}
