/* MIN (__builtin_ffs (X) - 1, C) becomes MIN (.CTZ (X, V), C) when clamping
   the all-ones result for X == 0 to C gives the same value as clamping
   V = .CTZ (0) to C.  */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-optimized" } */

#define MIN(a, b) ((a) < (b) ? (a) : (b))

unsigned
f1 (unsigned long x)
{
  unsigned t = __builtin_ffsl (x) - 1;
  return t < 7 ? t : 7;
}

unsigned
f2 (unsigned long x)
{
  unsigned t = (unsigned) __builtin_ffsl (x) - 1u;
  return t < 7 ? t : 7;
}

unsigned
f3 (unsigned x)
{
  unsigned t = __builtin_ffs (x) - 1;
  return MIN (t, 32u);
}

unsigned char
f4 (unsigned x)
{
  unsigned char t = __builtin_ffs (x) - 1;
  return MIN (t, 20);
}

unsigned long
f5 (unsigned long long x)
{
  unsigned long t = __builtin_ffsll (x) - 1;
  return MIN (t, 64ul);
}

unsigned
f6 (unsigned long x)
{
  unsigned t = __builtin_ffsl (x) - 1;
  return t > 7 ? 7 : t;
}

unsigned
f7 (unsigned long x)
{
  unsigned t = __builtin_ffsl (x);
  t--;
  return MIN (t, 9u);
}

int
f8 (unsigned long x)
{
  unsigned t = __builtin_ffsl (x) - 1;
  return t < 7 ? t : 7;
}

/* For X == 0 the original gives 65 while .CTZ (0) gives 64.  */
unsigned
g1 (unsigned long x)
{
  unsigned t = __builtin_ffsl (x) - 1;
  return MIN (t, 65u);
}

/* For X == 0 the signed subtraction gives -1, which .CTZ can't produce.  */
int
g2 (unsigned long x)
{
  int t = __builtin_ffsl (x) - 1;
  return t < 7 ? t : 7;
}

/* { dg-final { scan-tree-dump-times "\\.CTZ \\(" 8 "optimized" } } */
/* { dg-final { scan-tree-dump-times "__builtin_ffs" 2 "optimized" } } */
