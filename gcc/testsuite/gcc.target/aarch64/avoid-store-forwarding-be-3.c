/* { dg-do run } */
/* { dg-require-effective-target aarch64_big_endian } */
/* { dg-options "-O2 -favoid-store-forwarding --param=store-forwarding-max-distance=0" } */

/* The bit insert that replaces the forwarded load can't use a word subreg
   of the vector register on big-endian, and the fallback used to write
   the inserted bytes into a copy of that register instead of the
   register itself.  */

typedef __attribute__ ((__vector_size__ (32))) unsigned int V;

long long i;
V buf;
V g;

__attribute__ ((noipa)) void
foo (V *v)
{
  __builtin_memcpy (v, &i, 5);
  g *= *v;
}

int
main (void)
{
  union { V v; unsigned char c[32]; } t;
  unsigned char *b = (unsigned char *) &buf;
  unsigned char *ip = (unsigned char *) &i;
  int k;

  for (k = 0; k < 32; k++)
    b[k] = t.c[k] = 0xa0 + k;
  for (k = 0; k < 8; k++)
    ip[k] = 5 + 3 * k;
  for (k = 0; k < 5; k++)
    t.c[k] = ip[k];
  for (k = 0; k < 8; k++)
    g[k] = 7;

  V expected = t.v * 7;
  foo (&buf);
  for (k = 0; k < 8; k++)
    if (g[k] != expected[k])
      __builtin_abort ();
  return 0;
}
