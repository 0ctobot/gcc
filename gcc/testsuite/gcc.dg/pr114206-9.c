/* PR tree-optimization/114206 */
/* A callee whose result says whether a guess matches the address of a local
   lets the caller know the address once it branches on that result, even if
   the guess was a parameter all along.  The guess then must not reach a tail
   call while the local is still in use, whether the branch returns early or
   both of its arms go on to the tail call.  */
/* { dg-do run } */
/* { dg-require-effective-target lp64 } */
/* { dg-options "-O2 -fno-stack-protector -fdump-tree-tailc-details" } */

#define NOIPA __attribute__ ((noinline, noclone))

typedef unsigned long ul;

extern void abort (void);

#define RANGE 16384

int sink;

NOIPA ul
stack_pos (void)
{
  return (ul) __builtin_frame_address (0);
}

/* The address can only be in PAD if the frame of the caller is gone.  */
NOIPA int
reader (ul addr, int h)
{
  volatile char pad[RANGE];
  pad[0] = h;
  return addr >= (ul) &pad[0] && addr < (ul) &pad[RANGE];
}

NOIPA int
same (int *a, ul c)
{
  sink += *a;
  return (ul) a == c;
}

NOIPA int
early_return (ul c)
{
  int x[4];
  x[0] = 1;
  if (!same (&x[0], c))
    return 0;
  return reader (c, 1);
}

/* With H clear, the call only happens if the guess was right.  */
NOIPA int
both_arms (ul c, int h)
{
  int x[4];
  x[0] = 1;
  if (same (&x[0], c))
    {
      if (h)
	return 0;
    }
  else if (!h)
    return 0;
  return reader (c, h);
}

int
main (void)
{
  ul b = stack_pos ();
  for (ul c = b - 2 * RANGE; c < b + RANGE; c += 4)
    if (early_return (c) || both_arms (c, 0))
      abort ();
  return 0;
}

/* { dg-final { scan-tree-dump-not "Found tail call\[^\n\]*reader" "tailc" } } */
