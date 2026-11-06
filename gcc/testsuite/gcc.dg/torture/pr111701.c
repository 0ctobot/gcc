/* PR middle-end/111701 */
/* { dg-do run } */
/* { dg-additional-options "-lm" } */

/* Squaring a NaN can keep its sign, as it does on x86, so the sign bit of the
   result isn't known to be clear.  */

__attribute__((noipa)) static int
mult (float *x)
{
  *x *= *x;
  return __builtin_signbit (*x);
}

__attribute__((noipa)) static int
powi (float *x)
{
  *x = __builtin_powif (*x, 2);
  return __builtin_signbit (*x);
}

__attribute__((noipa)) static int
pow2 (double *x)
{
  *x = __builtin_pow (*x, 2.0);
  return __builtin_signbit (*x);
}

int
main (void)
{
  float a = -__builtin_nanf ("");
  float b = -__builtin_nanf ("");
  double c = -__builtin_nan ("");
  int s;

  s = mult (&a);
  if (!s != !__builtin_signbit (a))
    __builtin_abort ();
  s = powi (&b);
  if (!s != !__builtin_signbit (b))
    __builtin_abort ();
  s = pow2 (&c);
  if (!s != !__builtin_signbit (c))
    __builtin_abort ();
  return 0;
}
