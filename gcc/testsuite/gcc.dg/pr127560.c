/* PR c/127560 */
/* { dg-do run } */
/* { dg-options "-w" } */

/* These expressions aren't integer constant expressions since they divide by
   zero or shift by a negative count, yet the C front end still marks them as
   having integer operands.  Folding one of them used to leave a nested
   C_MAYBE_CONST_EXPR behind, which made the gimplifier ICE.  The program is
   valid since the division and the shifts never run: each function only takes
   the guarded path when P is nonzero, and main passes zero.  */

#define X ((1 > (1 >> (1 / 0))) ^ 1U ^ 1)

typedef _Bool b1;
void abort (void);
int g (int);
int g (int a) { return a; }

int f1 (int p) { if (p) return 1 > 1 >> (-1U >> -__INT_MAX__ ^ __INT_MAX__) ^ 1U ^ 1; return 1; }
int f2 (int p) { if (p) return X; return 1; }
int f3 (int p) { if (p) return (1 > (1 >> (1 % 0))) ^ 1U ^ 1; return 1; }
int f4 (int p) { if (p) return ((1 > (1 >> (1 / 0))) ^ 0U) ^ 1; return 1; }
int f5 (int p) { if (p) return ((1 > (1 >> (1 / 0))) ^ 1U) & 1; return 1; }
int f6 (int p) { if (p) return !X; return 1; }
int f7 (int p) { if (p) return -X; return 1; }
int f8 (int p) { if (p) return ~X; return 1; }
int f9 (int p) { if (p) return +X; return 1; }
int f10 (int p) { if (p) return X + 1; return 1; }
int f11 (int p) { if (p) return 1 << X; return 1; }
int f12 (int p) { if (p) return (char) X; return 1; }
int f13 (int p) { if (p) return (b1) X; return 1; }
int f14 (int p) { if (p) return X && 1; return 1; }
int f15 (int p) { if (p) return 1 && X; return 1; }
int f16 (int p) { if (p) return X, 1; return 1; }
int f17 (int p) { if (p) return 1, X; return 1; }
int f18 (int p) { if (p) return X ? 1 : 0; return 1; }
int f19 (int p) { if (p) return X ? 1 / 0 : 5; return 1; }
int f20 (int p) { if (p) return X ?: 5; return 1; }
int f21 (int p) { if (p) { int z = X; return z; } return 1; }
int f22 (int p) { if (p) { b1 z = X; return z; } return 1; }
int f23 (int p) { if (p) return g (X); return 1; }
int f24 (int p) { if (p) return __builtin_expect (X, 0); return 1; }
int f25 (int p) { if (p) { if (X) return 2; } return 1; }
int f26 (int p) { if (p) { while (X) return 2; } return 1; }
int f27 (int p) { if (p) { do { } while (X); } return 1; }
int f28 (int p) { if (p) { for (int i = X; i < 1; i++); } return 1; }
int f29 (int p) { if (p) { switch (X) { default: return 2; } } return 1; }
int f30 (int p) { if (p) { X; } return 1; }
int f31 (int p) { if (p) { int v[X]; return sizeof v; } return 1; }
int f32 (int p) { if (p) return ({ X; }); return 1; }
int f33 (int p) { if (p) return _Generic (1, int: X); return 1; }
int f34 (int p) { if (p) return __builtin_choose_expr (1, X, 0); return 1; }
float f35 (int p) { if (p) return X; return 1; }
unsigned long long f36 (int p) { if (p) return X; return 1; }
int f37 (int p) { if (p) return X || 0; return 1; }
int f38 (int p) { if (p) return 0 || X; return 1; }
int f39 (int p) { if (p) return (1 / 0) && X; return 1; }
int f40 (int p) { if (p) return (1 / 0) || X; return 1; }
int f41 (int p) { if (p) return ((1 > (1 >> (1 / 0))) ^ 1UL) ^ 1; return 1; }

int
main (void)
{
  if (f1 (0) != 1 || f2 (0) != 1 || f3 (0) != 1 || f4 (0) != 1
      || f5 (0) != 1 || f6 (0) != 1 || f7 (0) != 1 || f8 (0) != 1
      || f9 (0) != 1 || f10 (0) != 1 || f11 (0) != 1 || f12 (0) != 1
      || f13 (0) != 1 || f14 (0) != 1 || f15 (0) != 1 || f16 (0) != 1
      || f17 (0) != 1 || f18 (0) != 1 || f19 (0) != 1 || f20 (0) != 1
      || f21 (0) != 1 || f22 (0) != 1 || f23 (0) != 1 || f24 (0) != 1
      || f25 (0) != 1 || f26 (0) != 1 || f27 (0) != 1 || f28 (0) != 1
      || f29 (0) != 1 || f30 (0) != 1 || f31 (0) != 1 || f32 (0) != 1
      || f33 (0) != 1 || f34 (0) != 1 || f35 (0) != 1 || f36 (0) != 1
      || f37 (0) != 1 || f38 (0) != 1 || f39 (0) != 1 || f40 (0) != 1
      || f41 (0) != 1)
    abort ();
  return 0;
}
