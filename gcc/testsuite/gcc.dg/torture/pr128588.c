/* PR tree-optimization/128588 */
/* { dg-do run } */

/* If-to-switch conversion must not turn a bitwise or of two negated range
   tests into case ranges that take the false edge.  With disjoint ranges
   the condition is always true, so no call below may return 99.  */

#define CHAIN(x)		\
  if (x == 20) return 120;	\
  if (x == 21) return 221;	\
  if (x == 22) return 322;	\
  if (x == 23) return 423;	\
  if (x == 24) return 524;	\
  if (x == 25) return 625;	\
  if (x == 26) return 726;	\
  if (x == 27) return 827;	\
  if (x == 28) return 928;	\
  if (x == 29) return 1029;	\
  if (x == 30) return 1130;	\
  if (x == 31) return 1231;	\
  if (x == 32) return 1332;

__attribute__ ((noipa)) int
f1 (unsigned x)
{
  _Bool a = x - 3 > 4;
  _Bool b = x - 10 > 5;

  if (a | b)
    {
      CHAIN (x)
      return 1;
    }
  return 99;
}

static inline _Bool in_a (int c) { return c >= 3 && c <= 7; }
static inline _Bool in_b (int c) { return c >= 10 && c <= 15; }

__attribute__ ((noipa)) int
f2 (int c)
{
  if (!in_a (c) | !in_b (c))
    {
      CHAIN (c)
      return 1;
    }
  return 99;
}

int
main (void)
{
  for (int x = -2; x < 40; x++)
    if (f1 (x) == 99 || f2 (x) == 99)
      __builtin_abort ();
  return 0;
}
