/* { dg-do run } */

/* Check the values produced once the zero check before the builtin is
   removed.  WIDTH is the width of unsigned int, the type of the builtins'
   operands, and TOP is its top bit.  */

#define WIDTH (__SIZEOF_INT__ * __CHAR_BIT__)
#define TOP (1U << (WIDTH - 1))

__attribute__((noipa)) int
fls32 (unsigned int x)
{
  return x ? WIDTH - __builtin_clz (x) : 0;
}

__attribute__((noipa)) int
fls64 (unsigned long long x)
{
  if (!x)
    return 0;
  return 63 - __builtin_clzll (x) + 1;
}

__attribute__((noipa)) int
ilog2 (unsigned int x)
{
  return x ? WIDTH - 1 - __builtin_clz (x) : -1;
}

__attribute__((noipa)) int
ffs32 (unsigned int x)
{
  return x ? __builtin_ctz (x) + 1 : WIDTH + 1;
}

__attribute__((noipa)) int
pop (unsigned int x)
{
  return x ? __builtin_popcount (x) * 2 + 1 : 1;
}

__attribute__((noipa)) unsigned char
narrow (unsigned int x)
{
  return x ? (unsigned char) __builtin_clz (x) + 250
	   : (unsigned char) (WIDTH + 250);
}

__attribute__((noipa)) int
mismatch (unsigned int x)
{
  return x ? __builtin_popcount (x) + 1 : 0;
}

int
main ()
{
  if (fls32 (0) != 0 || fls32 (1) != 1 || fls32 (TOP) != WIDTH
      || fls32 (0x1234) != 13)
    __builtin_abort ();
  if (fls64 (0) != 0 || fls64 (1) != 1 || fls64 (1ULL << 63) != 64
      || fls64 (0x123456789ULL) != 33)
    __builtin_abort ();
  if (ilog2 (0) != -1 || ilog2 (1) != 0 || ilog2 (TOP) != WIDTH - 1)
    __builtin_abort ();
  if (ffs32 (0) != WIDTH + 1 || ffs32 (1) != 1 || ffs32 (TOP) != WIDTH
      || ffs32 (12) != 3)
    __builtin_abort ();
  if (pop (0) != 1 || pop (7) != 7 || pop (-1U) != 2 * WIDTH + 1)
    __builtin_abort ();
  if (narrow (0) != (unsigned char) (WIDTH + 250)
      || narrow (1) != (unsigned char) (WIDTH + 249) || narrow (-1U) != 250)
    __builtin_abort ();
  if (mismatch (0) != 0 || mismatch (1) != 2 || mismatch (3) != 3)
    __builtin_abort ();
  return 0;
}
