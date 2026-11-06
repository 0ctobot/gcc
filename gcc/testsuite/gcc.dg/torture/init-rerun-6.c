/* { dg-do run } */

/* An initializer that can be run again by a jump back is built in a temporary
   and copied into the variable.  Without any automatic variable initialization
   option, the padding of a member with no initializer must still come out zero,
   which the temporary has but a scalarized copy doesn't carry.  */

typedef __SIZE_TYPE__ size_t;

struct I { char a; long b; };
struct O { long x; struct I in; };

__attribute__ ((noipa)) long
g (long x)
{
  return x;
}

/* Leave a recognizable value in the part of the stack the tests go on to use.  */

__attribute__ ((noipa)) void
scribble (void)
{
  volatile unsigned char b[1024];
  for (int i = 0; i < 1024; i++)
    b[i] = 0x5a;
}

__attribute__ ((noipa)) void
check (const void *p, size_t n)
{
  const unsigned char *c = p;
  for (size_t i = 0; i < n; i++)
    if (c[i] == 0x5a)
      __builtin_abort ();
}

/* A label that no jump comes back to.  */

__attribute__ ((noipa)) void
forward_label (int n)
{
  if (n)
    goto skip;
  g (0);
skip:;
  struct O v = { .x = g (1) };
  check (&v, sizeof v);
}

/* A label that the initializer really runs again from.  */

__attribute__ ((noipa)) void
rerun (int n)
{
  int i = 0;
back:;
  struct O v = { .x = g (1 + i) };
  check (&v, sizeof v);
  if (i++ < n)
    goto back;
}

int
main (void)
{
  scribble ();
  forward_label (1);
  scribble ();
  rerun (1);
  return 0;
}
