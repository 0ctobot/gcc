/* { dg-do compile } */
/* { dg-require-effective-target lp64 } */
/* { dg-options "-O3 -fno-vect-cost-model" } */

/* WHILEWR is only wrong for a pair of masked references, which can both start
   right next to address 0 without accessing anything there.  A reference that
   isn't masked accesses its first lane, which keeps that lane's whole vector
   away from address 0.  */

void
unmasked (int *a, int *b, unsigned long n)
{
  for (unsigned long i = 0; i < n; i++)
    a[i] = b[i];
}

void
one_masked (int *a, int *b, unsigned long n)
{
  for (unsigned long i = 0; i < n; i++)
    if (b[i] > 0)
      a[i] = 1;
}

/* The pair of A and B is masked, while C runs in every iteration.  */

void
two_masked (int *a, int *b, int *c, unsigned long n)
{
  for (unsigned long i = 0; i < n; i++)
    if (c[i])
      a[i] = b[i];
}

/* { dg-final { scan-assembler-times {\twhilewr\tp[0-9]+\.s, x1, x0\n} 2 } } */
/* { dg-final { scan-assembler-times {\twhilewr\tp[0-9]+\.s, x2, x0\n} 1 } } */
/* { dg-final { scan-assembler-times {\twhilewr\t} 3 } } */
