/* { dg-do compile } */
/* { dg-additional-options "-O3 -fno-strict-aliasing -fno-vect-cost-model" } */

/* An array index counts whole structures no matter what the distance between
   two indices is, so the fields stay independent without any runtime check.  */

struct s3 { int x, y, z; };

struct s3 gs[10000];

void
index_long (long n, long d)
{
  for (long i = 8; i < n; i++)
    gs[i].y = gs[i + d].z;
}

void
index_int (int n, int d)
{
  for (int i = 8; i < n; i++)
    gs[i].y = gs[i + d].z;
}

/* { dg-final { scan-tree-dump-times "vectorized 1 loops" 2 "vect" { target aarch64*-*-* } } } */
/* { dg-final { scan-tree-dump-not "loop versioned" "vect" { target aarch64*-*-* } } } */
