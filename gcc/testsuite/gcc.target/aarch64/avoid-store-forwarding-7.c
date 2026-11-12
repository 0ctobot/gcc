/* { dg-do run } */
/* { dg-require-effective-target aarch64_little_endian } */
/* { dg-options "-O2 -favoid-store-forwarding --param=store-forwarding-max-distance=0" } */

/* Little-endian versions of avoid-store-forwarding-be-3.c and
   avoid-store-forwarding-be-4.c.  The 32-byte vector occupies a pair of
   vector registers, and only the low 64 bits of each register have a word
   subreg.  foo inserts bytes into the high 64 bits of the first register,
   and bar inserts bytes that straddle its two halves.  Both stores used to
   put the bytes for the high half into a copy of the register instead of
   the register itself.  */

typedef __attribute__ ((__vector_size__ (32))) unsigned int V;

long long i;
V buf;
V g;

__attribute__ ((noipa)) void
foo (V *v)
{
  __builtin_memcpy ((char *) v + 9, &i, 3);
  g *= *v;
}

__attribute__ ((noipa)) void
bar (V *v)
{
  __builtin_memcpy ((char *) v + 6, &i, 4);
  g *= *v;
}

static void
check (void (*fn) (V *), int off, int len)
{
  union { V v; unsigned char c[32]; } t;
  unsigned char *b = (unsigned char *) &buf;
  unsigned char *ip = (unsigned char *) &i;
  int k;

  for (k = 0; k < 32; k++)
    b[k] = t.c[k] = 0xa0 + k;
  for (k = 0; k < 8; k++)
    ip[k] = 5 + 3 * k;
  for (k = 0; k < len; k++)
    t.c[off + k] = ip[k];
  for (k = 0; k < 8; k++)
    g[k] = 7;

  V expected = t.v * 7;
  fn (&buf);
  for (k = 0; k < 8; k++)
    if (g[k] != expected[k])
      __builtin_abort ();
}

int
main (void)
{
  check (foo, 9, 3);
  check (bar, 6, 4);
  return 0;
}
