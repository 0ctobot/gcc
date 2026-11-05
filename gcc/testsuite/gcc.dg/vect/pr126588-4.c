/* { dg-additional-options "-O3" } */

#include "tree-vect.h"

/* The same dependence as pr81740-1.c, except through two pointers that may be
   the same object.  The dependence distance is negative in the outer loop and
   backward in the inner loop, so vectorizing the outer loop is invalid if
   they're the same object, and outer loops don't get runtime alias checks.  */

struct S { int a[8][10]; };

int c;

__attribute__((noipa)) void
foo (struct S *p, struct S *q)
{
  short b;
  for (b = 4; b >= 0; b--)
    for (c = 0; c <= 6; c++)
      p->a[c + 1][b + 2] = q->a[c][b + 1];
}

int
main (void)
{
  static struct S s;

  check_vect ();

  s.a[2][5] = 4;
  foo (&s, &s);
  for (int i = 0; i < 8; i++)
    for (int d = 0; d < 10; d++)
      if (s.a[i][d] != (i == 3 && d == 6) * 4)
	__builtin_abort ();

  return 0;
}
