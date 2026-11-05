/* PR tree-optimization/126565 */
/* { dg-do run } */
/* { dg-additional-options "-fno-tree-forwprop -fno-tree-vrp" } */

/* Negating a three-way comparison result in unsigned char only reverses the
   order when it's converted back to an 8-bit type.  A wider type holds the
   zero-extended negation, which maps less to 1, equal to 0, greater to 255 and
   unordered to 128.  */

#define TEST(name, type, pred)						\
__attribute__((noipa)) int						\
name (int x, int y)							\
{									\
  type c, g;								\
  unsigned char d, e;							\
  if (x == y)								\
    c = 0;								\
  else if (x < y)							\
    c = -1;								\
  else									\
    c = 1;								\
  d = c;								\
  e = -d;								\
  g = e;								\
  return pred;								\
}									\
__attribute__((noipa)) int						\
name##_fp (double x, double y)						\
{									\
  type c, g;								\
  unsigned char d, e;							\
  if (x == y)								\
    c = 0;								\
  else if (x < y)							\
    c = -1;								\
  else if (x > y)							\
    c = 1;								\
  else									\
    c = -128;								\
  d = c;								\
  e = -d;								\
  g = e;								\
  return pred;								\
}									\
__attribute__((noipa)) void						\
name##_check (void)							\
{									\
  static const double vals[] = { -1.0, 0.0, 1.0, __builtin_nan ("") };	\
  for (int i = 0; i < 4; i++)						\
    for (int j = 0; j < 4; j++)						\
      {									\
	double x = vals[i], y = vals[j];				\
	/* Compute the expected value without any negation to recognize.  */ \
	volatile type g = (type) (x == y ? 0 : x < y ? 1 : x > y ? 255 : 128); \
	if (name##_fp (x, y) != (pred))					\
	  __builtin_abort ();						\
	if (i < 3 && j < 3 && name (i, j) != (pred))			\
	  __builtin_abort ();						\
      }									\
}

#define TESTS(prefix, type)						\
  TEST (prefix##_gt0, type, g > 0)					\
  TEST (prefix##_ge1, type, g >= 1)					\
  TEST (prefix##_lt1, type, g < 1)					\
  TEST (prefix##_le0, type, g <= 0)					\
  TEST (prefix##_eq0, type, g == 0)					\
  TEST (prefix##_ne0, type, g != 0)					\
  TEST (prefix##_eq1, type, g == 1)					\
  TEST (prefix##_ne1, type, g != 1)					\
  TEST (prefix##_eqm1, type, g == -1)					\
  TEST (prefix##_nem1, type, g != -1)					\
  TEST (prefix##_gtm1, type, g > -1)					\
  TEST (prefix##_lem1, type, g <= -1)					\
  TEST (prefix##_lt0, type, g < 0)					\
  TEST (prefix##_ge0, type, g >= 0)

TESTS (sc, signed char)
TESTS (s, short)
TESTS (i, int)
TESTS (ll, long long)

#define CHECKS(prefix)							\
  do									\
    {									\
      prefix##_gt0_check ();						\
      prefix##_ge1_check ();						\
      prefix##_lt1_check ();						\
      prefix##_le0_check ();						\
      prefix##_eq0_check ();						\
      prefix##_ne0_check ();						\
      prefix##_eq1_check ();						\
      prefix##_ne1_check ();						\
      prefix##_eqm1_check ();						\
      prefix##_nem1_check ();						\
      prefix##_gtm1_check ();						\
      prefix##_lem1_check ();						\
      prefix##_lt0_check ();						\
      prefix##_ge0_check ();						\
    }									\
  while (0)

int
main ()
{
  CHECKS (sc);
  CHECKS (s);
  CHECKS (i);
  CHECKS (ll);
  return 0;
}
