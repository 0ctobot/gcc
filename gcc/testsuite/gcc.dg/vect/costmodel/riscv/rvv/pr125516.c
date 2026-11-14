/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d -O3 -fdump-tree-vect-details" } */

/* RVV gathers and scatters only take unsigned offsets, so the signed
   64-bit indices below are converted to unsigned 64-bit offsets.  That
   conversion doesn't need an instruction and mustn't be costed, or these
   barely profitable loops aren't vectorized.  */

#define N 1024

long a[N], b[N], idx[N];

void
f (void)
{
  for (int i = 0; i < N; i++)
    b[i] = a[idx[i]];
}

void
g (void)
{
  for (int i = 0; i < N; i++)
    b[idx[i]] = a[i];
}

/* { dg-final { scan-tree-dump-times "vectorized 1 loops" 2 "vect" } } */
