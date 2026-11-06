/* PR target/122582 */
/* { dg-do compile } */
/* { dg-options "-O3 -msve-vector-bits=256 -fno-vect-cost-model" } */

/* Each group of four chars is accessed as one 32-bit lane.  A 32-bit offset
   zero-extended to 64 bits cannot reach below the base address, nor can it
   stand in for a runtime step whose range is unknown.  */

void
runtime_step (signed char *a, int step, int n)
{
  for (int i = 0; i < n; i++)
    {
      a[i * step + 0] += 1;
      a[i * step + 1] += 2;
      a[i * step + 2] += 4;
      a[i * step + 3] += 8;
    }
}

void
negative_step (signed char *restrict a, signed char *restrict b, int n)
{
  for (int i = 0; i < n; i++)
    {
      a[i * -64 + 0] = b[i];
      a[i * -64 + 1] = b[i] + 1;
      a[i * -64 + 2] = b[i] + 2;
      a[i * -64 + 3] = b[i] + 3;
    }
}

/* { dg-final { scan-assembler {\tst1w\t[^\n]*, sxtw\]} } } */
/* { dg-final { scan-assembler-not {\t(?:ld1w|st1w)\t[^\n]*, uxtw\]} } } */
