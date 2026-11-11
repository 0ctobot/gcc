/* { dg-do run } */
/* { dg-additional-options "-fno-strict-aliasing" } */

/* A pointer one past the end of a struct is represented by the last member
   in points-to analysis.  Moving it back by one member gives a pointer to
   that last member, so the loads below read s.b or t.b.  Points-to analysis
   must not compute them to read the first member, or x is assumed to point
   only to l2 and the store through x is missed.  */

struct pair { int *a; int *b; };

__attribute__((noipa)) int
f1 (int c)
{
  int l1 = 0, l2 = 0;
  struct pair s = { &l2, &l1 }, t = { &l2, &l1 };
  int **p = c ? (int **) (&s + 1) : (int **) (&t + 1);
  int **q = p - 1;
  int *x = *q;
  l1 = 1;
  *x = 2;
  return l1;
}

__attribute__((noipa)) int
f2 (int c)
{
  int l1 = 0, l2 = 0;
  struct pair s = { &l2, &l1 }, t = { &l2, &l1 };
  int **p = c ? (int **) (&s + 1) : (int **) (&t + 1);
  int *x = p[-1];
  l1 = 1;
  *x = 2;
  return l1;
}

/* The pointer one past the end comes from memory.  */

__attribute__((noipa)) int
f3 (int c)
{
  int l1 = 0, l2 = 0;
  struct pair s = { &l2, &l1 }, t = { &l2, &l1 };
  int **ends[2] = { (int **) (&s + 1), (int **) (&t + 1) };
  int **p = ends[c];
  int *x = p[-1];
  l1 = 1;
  *x = 2;
  return l1;
}

/* The moved pointer is merged with another one before the load.  */

__attribute__((noipa)) int
f4 (int c, int d)
{
  int l1 = 0, l2 = 0, l3 = 0;
  struct pair s = { &l2, &l1 }, t = { &l2, &l1 };
  int *u = &l3;
  int **p = c ? (int **) (&s + 1) : (int **) (&t + 1);
  int **q = d ? p - 1 : &u;
  int *x = *q;
  l1 = 1;
  *x = 2;
  return l1;
}

int
main (void)
{
  if (f1 (1) != 2 || f1 (0) != 2)
    __builtin_abort ();
  if (f2 (1) != 2 || f2 (0) != 2)
    __builtin_abort ();
  if (f3 (1) != 2 || f3 (0) != 2)
    __builtin_abort ();
  if (f4 (1, 1) != 2 || f4 (0, 1) != 2)
    __builtin_abort ();
  return 0;
}
