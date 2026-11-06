/* { dg-do run } */
/* { dg-additional-options "-fzero-init-padding-bits=all" } */

/* With -fzero-init-padding-bits=all, the padding of an aggregate must come out
   zero for every form of initializer, including when it's built in a temporary
   because a jump back can run the initializer again.  */

typedef __SIZE_TYPE__ size_t;

struct U { char c; long l; };

__attribute__ ((noipa)) long
g (long x)
{
  return x;
}

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

__attribute__ ((noipa)) void
braces (int n)
{
  int i = 0;
back:;
  struct U v = { .c = g (1 + i), .l = g (2) };
  check (&v, sizeof v);
  if (i++ < n)
    goto back;
}

__attribute__ ((noipa)) void
conditional (int n)
{
  int i = 0;
back:;
  struct U v = n ? (struct U) { .c = g (1 + i) } : (struct U) { .l = g (2) };
  check (&v, sizeof v);
  if (i++ < n)
    goto back;
}

__attribute__ ((noipa)) void
comma (int n)
{
  int i = 0;
back:;
  struct U v = (g (3), (struct U) { .c = g (1 + i) });
  check (&v, sizeof v);
  if (i++ < n)
    goto back;
}

int
main (void)
{
  scribble ();
  braces (1);
  scribble ();
  conditional (1);
  scribble ();
  comma (1);
  return 0;
}
