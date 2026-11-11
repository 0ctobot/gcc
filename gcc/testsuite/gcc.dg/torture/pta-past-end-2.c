/* { dg-do run } */
/* { dg-additional-options "-fno-strict-aliasing" } */

/* A store through a pointer one past the end of a struct, moved back by one
   member, writes the last member.  Points-to analysis must not compute it
   to write the first member, or the load through pb is assumed to see only
   &l2 and the store through x is missed.  */

struct pair { int *a; int *b; };

__attribute__((noipa)) int
f (int c, int d)
{
  int l1 = 0, l2 = 0;
  struct pair s = { &l2, &l2 }, t = { &l2, &l2 };
  int **p = c ? (int **) (&s + 1) : (int **) (&t + 1);
  int **pb = d ? &s.b : &t.b;
  p[-1] = &l1;
  int *x = *pb;
  l1 = 1;
  *x = 2;
  return l1;
}

int
main (void)
{
  if (f (1, 1) != 2 || f (0, 0) != 2)
    __builtin_abort ();
  return 0;
}
