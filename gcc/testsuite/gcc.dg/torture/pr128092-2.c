/* PR ipa/128092 */
/* { dg-do run } */
/* { dg-additional-options "-fwrapv" } */

/* ABSU_EXPR takes a signed operand and has an unsigned result.  IPA bit-CP
   must extend the operand's lattice value and mask by the operand's sign, or
   else it treats a negative operand as already being its own absolute value
   and keeps the operand's known high bits in the result.  */

__attribute__ ((noinline)) static unsigned
absu_callee (unsigned x)
{
  return x;
}

__attribute__ ((noinline)) static unsigned
absu_caller (int x)
{
  unsigned u = x;
  if (x < 0)
    u = -u;
  return absu_callee (u);
}

__attribute__ ((noinline)) static int
abs_short_callee (short x)
{
  return x;
}

__attribute__ ((noinline)) static int
abs_short_caller (short x)
{
  return abs_short_callee (x < 0 ? -x : x);
}

int
main (void)
{
  if (absu_caller (-1) != 1u)
    __builtin_abort ();
  if (absu_caller (-__INT_MAX__ - 1) != (unsigned) __INT_MAX__ + 1)
    __builtin_abort ();

  if (abs_short_caller (-1) != 1)
    __builtin_abort ();
  if (abs_short_caller (-__SHRT_MAX__ - 1) != -__SHRT_MAX__ - 1)
    __builtin_abort ();

  return 0;
}
