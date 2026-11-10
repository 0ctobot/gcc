/* { dg-do compile } */
/* { dg-options "-O2" } */

/* For "if (x >= n) x -= n" the comparison reads the operand the subtraction
   updates.  noce_try_cond_arith must leave such a block to
   noce_try_cmove_arith, which folds the comparison into the subtraction
   (subs and csel), rather than emit cmp, a csel against zero and sub.  */

unsigned
r1 (unsigned x, unsigned n)
{
  if (x >= n)
    x -= n;
  return x;
}

unsigned long
r2 (unsigned long x, unsigned long n)
{
  if (x >= n)
    x -= n;
  return x;
}

unsigned
r3 (unsigned x, unsigned n)
{
  x++;
  if (x >= n)
    x -= n;
  return x;
}

/* { dg-final { scan-assembler-times {\tsubs\t} 3 } } */
/* { dg-final { scan-assembler-not {\tcmp\t} } } */
/* { dg-final { scan-assembler-not {\tcsel\t[wx][0-9]+, [wx][0-9]+, [wx]zr} } } */
