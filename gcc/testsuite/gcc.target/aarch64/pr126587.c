/* { dg-do run } */
/* { dg-options "-O3 -std=gnu11 -mgeneral-regs-only" } */

/* Without vector registers the only way to reach a vectorization factor of 1
   for these 64-bit accesses is a vector(1) long held in a general register.
   The 16-byte vector mode needs more alias checks than the limit allows, which
   makes the DImode loop the one that gets picked.  The loads through P, Q and
   R are hoisted out of it, so the stores to B, C, D and E need runtime alias
   checks against them even though the factor is 1.  */

__attribute__((noipa)) void
foo (int *a, unsigned long *b, unsigned long *c, unsigned long *d,
     unsigned long *e, unsigned long *p, unsigned long *q, unsigned long *r,
     unsigned long n)
{
  for (unsigned long i = 0; i < n; i++)
    {
      a[2 * i] = 1;
      a[2 * i + 1] = 2;
      c[i] = *p;
      b[i] = *q;
      d[i] = *r;
      e[i] = 9;
    }
}

int a[64];
unsigned long b[8], c[8], d[8], e[8], q = 77, r = 88;

int
main (void)
{
  for (int i = 0; i < 8; i++)
    {
      b[i] = 5 + i;
      c[i] = -1;
    }

  foo (a, b, c, d, e, &b[0], &q, &r, 8);

  /* The first iteration reads b[0] before writing it and every later
     iteration sees the value of *q stored by the first one.  */
  if (c[0] != 5)
    __builtin_abort ();
  for (int i = 1; i < 8; i++)
    if (c[i] != 77)
      __builtin_abort ();

  return 0;
}
