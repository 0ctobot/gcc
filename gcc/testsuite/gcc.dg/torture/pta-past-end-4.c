/* { dg-do run } */
/* { dg-additional-options "-fno-strict-aliasing" } */

/* A pointer into the padding after s.a, moved forward by 8 bytes, points
   to s.b.  Points-to analysis must not compute the load through it to read
   s.c, or x is assumed to point only to l2 and the store through x is
   missed.  */

struct pad { int *a; _Alignas (16) int *b; int *c; };

__attribute__((noipa)) int
f (int c)
{
  int l1 = 0, l2 = 0;
  struct pad s = { &l2, &l1, &l2 }, t = { &l2, &l1, &l2 };
  char *p = c ? (char *) &s + 8 : (char *) &t + 8;
  int *x = *(int **) (p + 8);
  l1 = 1;
  *x = 2;
  return l1;
}

int
main (void)
{
  if (f (1) != 2 || f (0) != 2)
    __builtin_abort ();
  return 0;
}
