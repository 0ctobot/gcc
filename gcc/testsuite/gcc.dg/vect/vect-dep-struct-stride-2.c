/* { dg-do compile } */
/* { dg-additional-options "-O3 -fno-strict-aliasing -fno-strict-overflow -fno-vect-cost-model" } */

/* Stepping by an ordinary multiple of the size of the structure can't reach a
   partial overlap, which means that the fields of these loops stay
   independent.  */

struct s { long x, y, z; };

void
h (struct s *restrict p, long *restrict q, long n)
{
  for (long i = 0; i < n; i++)
    {
      p[i].x = q[i];
      p[i].y = q[i] + 1;
      p[i].z = 3;
    }
}

void
z (struct s *p, long n)
{
  for (long i = 0; i < n; i++)
    {
      p[i * 2].x += 1;
      p[i * 2].y += 2;
    }
}

/* { dg-final { scan-tree-dump-times "vectorized 1 loops" 2 "vect" { target aarch64*-*-* } } } */
