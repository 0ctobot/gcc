/* PR tree-optimization/127345 */
/* Padding bits of a _BitInt can only hold arbitrary values on targets whose
   ABI leaves them undefined.  */
/* { dg-do run { target { bitint && { aarch64*-*-* i?86-*-* x86_64-*-* } } } } */
/* { dg-options "-std=gnu23 -fgimple" } */

struct S { _BitInt (3) c; };
union U { float f; _BitInt (30) b; };

/* Reading the member has to ignore the padding bits of a copied object.  */

[[gnu::noipa]] long long
f1 (_BitInt (3) x, struct S *p, int c)
{
  struct S s;
  if (c)
    s.c = x;
  else
    s = *p;
  return s.c;
}

/* The same goes for bits that come from an object that isn't an integer.  */

[[gnu::noipa]] long long
f2 (float x)
{
  union U u;
  u.f = x;
  return u.b;
}

/* A VIEW_CONVERT_EXPR to fewer bits of precision truncates, like a cast.  */

long long __GIMPLE (ssa) __attribute__((noipa))
f3 (_BitInt (8) x)
{
  _BitInt (3) y;
  long long r;

  __BB(2):
  y_2 = __VIEW_CONVERT <_BitInt (3)> (x_1(D));
  r_3 = (long long) y_2;
  return r_3;
}

int
main ()
{
  unsigned char b = 7;
  struct S t;
  __builtin_memcpy (&t, &b, 1);
  if (f1 (1, &t, 0) != -1)
    __builtin_abort ();
  /* -1.0f is 0xbf800000, which has 0x3f800000 in its low 30 bits, and bit 29
     of that makes the _BitInt (30) negative.  */
  if (f2 (-1.0f) != 0x3f800000LL - 0x40000000LL)
    __builtin_abort ();
  if (f3 (7) != -1)
    __builtin_abort ();
}
