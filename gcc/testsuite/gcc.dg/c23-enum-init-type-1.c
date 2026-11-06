/* While an enumeration is being parsed, C23 gives a constant that int can't
   represent the type of the expression initializing it, or for a constant
   without an initializer, the type of the previous constant.  That type can be
   an enumerated type, which is distinct from another one with the same
   underlying type.  */
/* { dg-do compile } */
/* { dg-options "-std=c23 -pedantic-errors" } */

enum P { p1 = -1U };
enum Z { z1 = -1U };
enum F : unsigned long long { f1 = -1ULL };

#define IS_P(x) _Generic (x, enum P: 1, enum Z: 2, default: 3)

enum A
{
  a1 = (enum P) -1U,
  a2 = IS_P (a1),
  a3 = sizeof (a1)
};
static_assert (a2 == 1);
static_assert (a3 == sizeof (enum P));
static_assert (_Generic (a1, enum A: 1, default: 0));

/* A constant naming an enumerator of another enumeration has its type.  */
enum B
{
  b1 = p1,
  b2 = IS_P (b1),
  b3 = _Generic (z1, enum Z: 4, default: 5),
  b4 = z1,
  b5 = _Generic (b4, enum Z: 6, default: 7)
};
static_assert (b2 == 1 && b3 == 4 && b5 == 6);

/* The next constant has the type of the previous one without an overflow.  */
enum C
{
  c1 = (enum P) (-1U - 1),
  c2,
  c3 = IS_P (c2),
  c4 = c2 == -1U
};
static_assert (c3 == 1 && c4 == 1);

/* If it does overflow, it has a wider type of the same signedness.  */
enum D
{
  d1 = (enum P) -1U,
  d2,
  d3 = IS_P (d2),
  d4 = sizeof (d2) > sizeof (enum P),
  d5 = d2 == 1ULL + -1U
};
static_assert (d3 == 3 && d4 == 1 && d5 == 1);

/* A constant that int can represent has type int.  */
enum E
{
  e1 = (enum P) 1,
  e2 = _Generic (e1, int: 1, default: 2),
  e3 = (enum P) 0x7fffffff,
  e4 = _Generic (e3, int: 1, default: 2)
};
static_assert (e2 == 1 && e4 == 1);

/* The same goes for an enumerated type with a fixed underlying type.  */
enum G
{
  g1 = (enum F) -1ULL,
  g2 = _Generic (g1, enum F: 1, default: 2)
};
static_assert (g2 == 1);

/* A signed underlying type goes through the range of int as well.  */
enum N : long long { n1 = -0x80000002LL };
enum H
{
  h1 = (enum N) -0x80000002LL,
  h2 = _Generic (h1, enum N: 1, default: 2),
  h3 = (enum N) -0x80000001LL,
  h4,
  h5 = _Generic (h4, int: 1, default: 2),
  h6 = h4 == -0x7fffffff - 1
};
static_assert (h2 == 1 && h5 == 1 && h6 == 1);

/* Other types of initializer keep working as before.  */
enum I
{
  i1 = -1U,
  i2 = _Generic (i1, unsigned int: 1, default: 2)
};
static_assert (i2 == 1);

/* A redeclaration can name the same values through such a constant.  */
enum J { j1 = (enum P) -1U, j2 };
enum J { j1 = (enum P) -1U, j2 };
enum K { k1 = -1U };
enum K { k1 = (enum P) -1U };
