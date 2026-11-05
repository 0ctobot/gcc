/* PR c/120862 */
/* { dg-do compile } */
/* { dg-options "-std=gnu23 -O0" } */
/* { dg-timeout 60 } */

/* Each struct here is variably modified only since it points to the one before
   it, and it does so twice.  None of them depends on a variable of the nested
   function, so finding that out for the last one must not walk the first one
   once for every path to it.  */

#define P(a, b) struct T##b { struct T##a *p, *q; };

int
outer (int n, void *v)
{
  struct T0 { int a[n]; };
  P (0, 1)
  P (1, 2)
  P (2, 3)
  P (3, 4)
  P (4, 5)
  P (5, 6)
  P (6, 7)
  P (7, 8)
  P (8, 9)
  P (9, 10)
  P (10, 11)
  P (11, 12)
  P (12, 13)
  P (13, 14)
  P (14, 15)
  P (15, 16)
  P (16, 17)
  P (17, 18)
  P (18, 19)
  P (19, 20)
  P (20, 21)
  P (21, 22)
  P (22, 23)
  P (23, 24)
  P (24, 25)
  P (25, 26)
  P (26, 27)
  P (27, 28)
  P (28, 29)
  P (29, 30)
  P (30, 31)
  P (31, 32)
  P (32, 33)
  P (33, 34)
  P (34, 35)
  P (35, 36)
  P (36, 37)
  P (37, 38)
  P (38, 39)
  P (39, 40)

  __attribute__ ((always_inline)) inline int nested (void *w)
  {
    struct T40 *y = w;
    return y == 0;
  }
  return nested (v);
}
