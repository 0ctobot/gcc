/* { dg-do run } */
/* { dg-options "-O3 -mgeneral-regs-only -fno-strict-aliasing" } */

/* ivdep rules out dependences between iterations, but it says nothing about the
   accesses in one iteration.  Without SIMD registers the vectorizer still uses
   a pair of X registers as a 16-byte vector for each group of byte accesses.  A
   group of loads and a group of stores would swap the load of each P byte with
   the store to the same Q byte unless the loop checks that P and Q don't
   overlap.  */

struct T { unsigned char b[8]; };

__attribute__((noipa)) void
foo (struct T *restrict o, struct T *p, struct T *q, int n)
{
#pragma GCC ivdep
  for (int i = 0; i < n; i++)
    {
      q[i].b[0] = 1;
      o[i].b[0] = p[i].b[0];
      q[i].b[1] = 2;
      o[i].b[1] = p[i].b[1];
      q[i].b[2] = 3;
      o[i].b[2] = p[i].b[2];
      q[i].b[3] = 4;
      o[i].b[3] = p[i].b[3];
      q[i].b[4] = 5;
      o[i].b[4] = p[i].b[4];
      q[i].b[5] = 6;
      o[i].b[5] = p[i].b[5];
      q[i].b[6] = 7;
      o[i].b[6] = p[i].b[6];
      q[i].b[7] = 8;
      o[i].b[7] = p[i].b[7];
    }
}

/* The vector code requires 16-byte alignment.  */
_Alignas (16) struct T a[64], r[64];

int
main (void)
{
  foo (r, a, a, 32);
  for (int i = 0; i < 32; i++)
    for (int j = 0; j < 8; j++)
      if (r[i].b[j] != j + 1)
	__builtin_abort ();

  return 0;
}
