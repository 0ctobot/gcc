/* PR tree-optimization/114206 */
/* A callee can tell the address of a local through its result without keeping
   the address, whether the caller only branches on that result or the result is
   a pointer.  A search over the stack can then find the address, which must not
   reach a tail call while the local is still in use.  */
/* { dg-do run } */
/* { dg-require-effective-target lp64 } */
/* { dg-options "-O2 -fno-stack-protector" } */

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

/* If the frame of the caller is gone then this overwrites what it held.  */
NOIPA long
reader (ul addr)
{
  volatile char pad[RANGE];
  for (int i = 0; i < RANGE; i++)
    pad[i] = 0x55;
  return *(int *) addr;
}

NOIPA int
same (int *a, ul c)
{
  sink += *a;
  return (ul) a == c;
}

NOIPA long
branched_on (void)
{
  int x[256];
  x[128] = 1;
  ul b = stack_pos ();
  for (ul c = b; c < b + RANGE; c += 4)
    if (same (&x[128], c))
      return reader (c);
  return -1;
}

NOIPA void *
look (int *a, ul c)
{
  sink += *a;
  return (ul) a == c ? (void *) c : (void *) 0;
}

NOIPA long
pointer_result (void)
{
  int x[256];
  x[128] = 1;
  ul b = stack_pos ();
  for (ul c = b; c < b + RANGE; c += 4)
    {
      void *r = look (&x[128], c);
      if (r)
	return reader ((ul) r);
    }
  return -1;
}

NOIPA ul
difference (int *a, ul c)
{
  sink += *a;
  return (ul) a ^ c;
}

NOIPA long
zero_difference (void)
{
  int x[256];
  x[128] = 1;
  ul b = stack_pos ();
  for (ul c = b; c < b + RANGE; c += 4)
    if (!difference (&x[128], c))
      return reader (c);
  return -1;
}

int
main (void)
{
  if (branched_on () != 1
      || pointer_result () != 1
      || zero_difference () != 1)
    abort ();
  return 0;
}
