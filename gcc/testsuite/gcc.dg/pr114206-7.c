/* PR tree-optimization/114206 */
/* Each activation has its own copy of a parameter passed by value.  A pure
   function can tell the address of that copy through a result that's only
   branched on, so a search over the stack lets the next activation see whether
   its copy shares the storage of the previous one.  */
/* { dg-do run } */
/* { dg-require-effective-target lp64 } */
/* { dg-options "-O2 -fno-stack-protector" } */

#define NOIPA __attribute__ ((noinline, noclone))

typedef unsigned long ul;

extern void abort (void);

struct s { int v; };

NOIPA ul
stack_pos (void)
{
  return (ul) __builtin_frame_address (0);
}

__attribute__ ((noinline, noclone, pure)) int
same (void *p, ul c)
{
  return (ul) p == c;
}

NOIPA int
rec (struct s a, int n, ul prev)
{
  ul me = 0, b = stack_pos ();
  for (ul c = b; c < b + 4096; c += 4)
    if (same (&a, c))
      {
	me = c;
	break;
      }
  if (me && me == prev)
    return -1;
  if (n == 0)
    return 0;
  return rec (a, n - 1, me);
}

int
main (void)
{
  struct s y = { 1 };
  if (rec (y, 3, 0) != 0)
    abort ();
  return 0;
}
