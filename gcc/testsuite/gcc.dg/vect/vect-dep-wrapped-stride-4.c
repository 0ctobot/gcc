/* { dg-do compile } */
/* { dg-additional-options "-O3 -fno-strict-aliasing -fno-strict-overflow -fno-vect-cost-model" } */

/* Both references of each loop step by the size of the structure from offsets
   that are a whole number of structures apart, so the fields of these loops
   stay independent.  */

struct s { long x, y, z; };

void
g (struct s *p, long n)
{
  for (long i = 0; i < n; i++)
    p[i].x = p[i + 2].y + 1;
}

void
o (struct s *p, long n, long off)
{
  for (long i = 0; i < n; i++)
    {
      p[i + off].x += 1;
      p[i + off].y += 2;
    }
}

/* { dg-final { scan-tree-dump-times "vectorized 1 loops" 2 "vect" { target aarch64*-*-* } } } */
