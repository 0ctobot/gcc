/* PR tree-optimization/114206 */
/* Values narrower than a pointer can still carry the address of a local that
   a callee revealed through its result, since a few of them are enough to
   rebuild the address.  */
/* { dg-do run } */
/* { dg-require-effective-target lp64 } */
/* { dg-options "-O2 -fno-stack-protector -fdump-tree-tailc-details" } */

#define NOIPA __attribute__ ((noinline, noclone))

typedef unsigned long ul;
typedef unsigned int u32;

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
reader (u32 lo, u32 hi)
{
  volatile char pad[RANGE];
  ul addr = (ul) hi << 32 | lo;
  pad[0] = 0;
  return addr >= (ul) &pad[0] && addr < (ul) &pad[RANGE];
}

NOIPA void *
look (int *a, ul c)
{
  sink += *a;
  return (ul) a == c ? (void *) c : (void *) 0;
}

NOIPA int
halves (ul c)
{
  int x[4];
  x[0] = 1;
  void *r = look (&x[0], c);
  return reader ((u32) (ul) r, (u32) ((ul) r >> 32));
}

__attribute__ ((noinline, noclone, const)) int
address_bit (const void *p, unsigned int i)
{
  return ((ul) p >> i) & 1;
}

/* Each level has its own LOCAL, so the halves can't match those of the level
   before unless the recursion reuses the frame.  */
NOIPA int
recurse (unsigned int n, u32 prev_lo, u32 prev_hi)
{
  int local;
  u32 lo = 0, hi = 0;
  for (unsigned int i = 0; i < 32; i++)
    {
      if (address_bit (&local, i))
	{
	  __asm__ __volatile__ ("");
	  lo |= 1u << i;
	}
      if (address_bit (&local, i + 32))
	{
	  __asm__ __volatile__ ("");
	  hi |= 1u << i;
	}
    }
  if (n == 0)
    return lo == prev_lo && hi == prev_hi;
  return recurse (n - 1, lo, hi);
}

int
main (void)
{
  ul b = stack_pos ();
  for (ul c = b - 2 * RANGE; c < b + RANGE; c += 4)
    if (halves (c))
      abort ();
  if (recurse (1, 0, 0))
    abort ();
  return 0;
}

/* { dg-final { scan-tree-dump-not "Found tail call\[^\n\]*reader" "tailc" } } */
