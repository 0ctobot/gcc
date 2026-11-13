/* Check MIN (__builtin_ffs (X) - 1, C) for zero, every single bit, all ones
   and a few mixed values, across the int, long and long long widths.  */
/* { dg-do run } */

#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define NOIPA __attribute__ ((noipa))

NOIPA unsigned
f1 (unsigned long x)
{
  unsigned t = __builtin_ffsl (x) - 1;
  return t < 7 ? t : 7;
}

NOIPA unsigned
f2 (unsigned long x)
{
  unsigned t = (unsigned) __builtin_ffsl (x) - 1u;
  return t < 7 ? t : 7;
}

NOIPA unsigned
f3 (unsigned x)
{
  unsigned t = __builtin_ffs (x) - 1;
  return MIN (t, 32u);
}

NOIPA unsigned char
f4 (unsigned x)
{
  unsigned char t = __builtin_ffs (x) - 1;
  return MIN (t, 20);
}

NOIPA unsigned long
f5 (unsigned long long x)
{
  unsigned long t = __builtin_ffsll (x) - 1;
  return MIN (t, 64ul);
}

NOIPA unsigned
f6 (unsigned long x)
{
  unsigned t = __builtin_ffsl (x) - 1;
  return t > 7 ? 7 : t;
}

NOIPA unsigned
f7 (unsigned long x)
{
  unsigned t = __builtin_ffsl (x);
  t--;
  return MIN (t, 9u);
}

NOIPA int
f8 (unsigned long x)
{
  unsigned t = __builtin_ffsl (x) - 1;
  return t < 7 ? t : 7;
}

NOIPA unsigned
g1 (unsigned long x)
{
  unsigned t = __builtin_ffsl (x) - 1;
  return MIN (t, 65u);
}

NOIPA int
g2 (unsigned long x)
{
  int t = __builtin_ffsl (x) - 1;
  return t < 7 ? t : 7;
}

/* Reference ffs that doesn't use any bit scan builtin.  */
static NOIPA int
ref_ffs (unsigned long long x, int prec)
{
  for (int i = 0; i < prec; i++)
    if ((x >> i) & 1)
      return i + 1;
  return 0;
}

#define LPREC (int) (__CHAR_BIT__ * sizeof (long))
#define LLPREC (int) (__CHAR_BIT__ * sizeof (long long))
#define IPREC (int) (__CHAR_BIT__ * sizeof (int))

static void
check (unsigned long long v)
{
  unsigned long l = v;
  unsigned i = v;
  unsigned tl = ref_ffs (l, LPREC) - 1;
  unsigned ti = ref_ffs (i, IPREC) - 1;
  unsigned char tc = ref_ffs (i, IPREC) - 1;
  unsigned long tll = ref_ffs (v, LLPREC) - 1;
  int sl = ref_ffs (l, LPREC) - 1;

  if (f1 (l) != MIN (tl, 7u))
    __builtin_abort ();
  if (f2 (l) != MIN (tl, 7u))
    __builtin_abort ();
  if (f3 (i) != MIN (ti, 32u))
    __builtin_abort ();
  if (f4 (i) != MIN (tc, 20))
    __builtin_abort ();
  if (f5 (v) != MIN (tll, 64ul))
    __builtin_abort ();
  if (f6 (l) != MIN (tl, 7u))
    __builtin_abort ();
  if (f7 (l) != MIN (tl, 9u))
    __builtin_abort ();
  if (f8 (l) != (int) MIN (tl, 7u))
    __builtin_abort ();
  if (g1 (l) != MIN (tl, 65u))
    __builtin_abort ();
  if (g2 (l) != MIN (sl, 7))
    __builtin_abort ();
}

int
main (void)
{
  check (0);
  check (~0ULL);
  for (int b = 0; b < LLPREC; b++)
    {
      check (1ULL << b);
      check (~0ULL << b);
      check ((1ULL << b) | (1ULL << (LLPREC - 1)));
    }
  check (0x50);
  check (0xf00);
  check (0x123456789abcdef0ULL);
  return 0;
}
